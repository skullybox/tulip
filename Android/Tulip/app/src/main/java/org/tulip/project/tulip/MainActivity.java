package org.tulip.project.tulip;

import android.app.ActionBar;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.support.design.widget.FloatingActionButton;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.text.InputType;
import android.view.*;
import android.widget.*;

import java.math.BigInteger;
import java.util.*;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

public class MainActivity extends AppCompatActivity implements Runnable {

    Userlist adaptor;
    Thread _main_tulip;
    String[] friendRequestList = null;
    public static List<Object> friends = Collections.synchronizedList(new ArrayList<>());;
    public static List<Object> friends_message = Collections.synchronizedList(new ArrayList<>()); // list of users with new messages
    public static List<Object> friendsRequest = Collections.synchronizedList(new ArrayList<>());
    int[] stateimage = {R.drawable.grey, R.drawable.pink};
    public static ConcurrentHashMap<String, ArrayList<Message>> messages = new ConcurrentHashMap<>();
    ListView listview;
    AlertDialog.Builder freqDialog;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        freqDialog = new AlertDialog.Builder(this);

        listview = (ListView) findViewById(R.id.listView);
        FloatingActionButton fab = (FloatingActionButton) findViewById(R.id.floatingActionButton2);

        fab.setVisibility(View.INVISIBLE);
        listview.setDivider(null);
        listview.setDividerHeight(0);

        ArrayList<String> _t = new ArrayList<String>();
        Object[] _data = MainActivity.friends.toArray();

        for(Object o:_data){
            _t.add(o.toString());
        }

        listview.fling(2);

        adaptor = new Userlist(this, _t);
        listview.setAdapter(adaptor);

        // fab click listener
        addClickListener();

        _main_tulip = new Thread(this);
        _main_tulip.start();

        getSupportActionBar().setDisplayHomeAsUpEnabled(false);

    }



    private void addClickListener() {
        FloatingActionButton fab = (FloatingActionButton) findViewById(R.id.floatingActionButton2);
        fab.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View view) {

                freqDialog.setTitle("Friend Request: " + friendsRequest.get(0));

                freqDialog.setPositiveButton("Accept", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        FloatingActionButton fab = (FloatingActionButton) findViewById(R.id.floatingActionButton2);
                        int ret = AcceptFriend(TulipSession.user, TulipSession.password,
                                (String)friendsRequest.get(0));
                        if (ret == 0) {
                            friends.add(friendsRequest.get(0));

                            runOnUiThread(new Runnable() {
                                @Override
                                public void run() {
                                    adaptor.notifyDataSetChanged();
                                }
                            });

                            friendsRequest.remove(0);
                        }
                        if (friendsRequest.size() == 0)
                            fab.setVisibility(View.INVISIBLE);
                        else
                            fab.setVisibility(View.VISIBLE);
                    }
                });
                freqDialog.setNegativeButton("Ignore", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        FloatingActionButton fab = (FloatingActionButton) findViewById(R.id.floatingActionButton2);
                        IgnoreFriend(TulipSession.user, TulipSession.password,
                                (String)friendsRequest.get(0));
                        friendsRequest.remove(0);
                        if (friendsRequest.size() == 0)
                            fab.setVisibility(View.INVISIBLE);
                        else
                            fab.setVisibility(View.VISIBLE);
                    }
                });

                freqDialog.show();
            }
        });

        ListView userList = (ListView) findViewById(R.id.listView);
        userList.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> adapterView, View view, int i, long l) {
                String itm = adapterView.getItemAtPosition(i).toString();
                TulipSession.current_chat_user = itm;
                Intent intent = new Intent(MainActivity.this, Chat.class);
                startActivity(intent);
            }
        });

    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        ActionBar actionBar = getActionBar();
        if (actionBar != null) {
            actionBar.setHomeButtonEnabled(false); // disable the button
            actionBar.setDisplayHomeAsUpEnabled(false); // remove the left caret
            actionBar.setDisplayShowHomeEnabled(false); // remove the icon
        }
        getMenuInflater().inflate(R.menu.chat_options, menu);
        return super.onCreateOptionsMenu(menu);
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {

        switch (item.getItemId()) {
            case R.id.AddUser:
                AlertDialog.Builder builder = new AlertDialog.Builder(this);
                builder.setTitle("Friend Request");
                final EditText input = new EditText(this);
                input.setInputType(InputType.TYPE_CLASS_TEXT);
                builder.setView(input);
                builder.setPositiveButton("Send", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        String friendRequest = input.getText().toString();
                        FriendRequest(TulipSession.user,
                                TulipSession.password,
                                friendRequest);

                    }
                });
                builder.setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        dialog.cancel();
                    }
                });

                builder.show();
                return true;
            case R.id.ChangePass:
                return true;
            case R.id.Logout:
                _main_tulip.interrupt();

                while(_main_tulip.isAlive()) { _main_tulip.interrupt();}

                Logout(TulipSession.user, TulipSession.password);
                TulipSession.user = "";
                TulipSession.password = "";
                TulipSession.current_chat_user = "";
                TulipSession.current_chat_user = "";
                TulipSession.current_offset = BigInteger.ZERO;

                messages.clear();
                friends_message.clear();
                friendsRequest.clear();
                friends.clear();
                finish();
                return true;
            default:
                return super.onOptionsItemSelected(item);
        }

    }

    @Override
    public void onBackPressed() {
        moveTaskToBack(true);
    }

    @Override
    public void run() {
        android.os.Process.setThreadPriority(android.os.Process.THREAD_PRIORITY_BACKGROUND);
        final ListView listview = (ListView) findViewById(R.id.listView);
        final FloatingActionButton fab = (FloatingActionButton) findViewById(R.id.floatingActionButton2);

        int indexRet = 0;
        int count = 0;

        long current_message_timeout = 30; // milliseconds
        long friend_request_last_check = System.currentTimeMillis() / 1000L - 20;
        long message_last_check = System.currentTimeMillis() / 1000L - 10;

        while (true) {

            if(_main_tulip.isInterrupted())
                break;

            if(count >= 7) {
                try {
                    Thread.sleep(300);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
            else
            {
                try {
                    Thread.sleep(30);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }

            // check for friend requests
            if (System.currentTimeMillis() / 1000L - friend_request_last_check > 17)
                if (friendRequestList == null || friendRequestList.length == 0) {

                    friendRequestList = GetFriendRequest(TulipSession.user, TulipSession.password);

                    if (friendRequestList != null && friendRequestList.length > 0) {
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                // This code will always run on the UI thread, therefore is safe to modify UI elements.
                                fab.setVisibility(View.VISIBLE);
                            }
                        });

                    } else {
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                // This code will always run on the UI thread, therefore is safe to modify UI elements.
                                fab.setVisibility(View.INVISIBLE);
                            }
                        });
                    }

                    if(friendRequestList != null)
                        for (String _f : friendRequestList) {
                            friendsRequest.add(_f);
                        }

                    friend_request_last_check = System.currentTimeMillis() / 1000L;
                }

            // check for friends messages
            if (System.currentTimeMillis() - message_last_check > current_message_timeout) {

                message_last_check = System.currentTimeMillis();

                String target_user_list = "";
                boolean flag_update_user_list = false;
                boolean current_user_message = false;
                Message _msg = GetMessage(TulipSession.user, TulipSession.password, "",
                        TulipSession.current_offset.toString());

                if (_msg == null) {
                    count++;
                    continue;
                }
                else
                {
                    count = 0;
                }

                if (_msg.getId().compareTo(TulipSession.current_offset) == 1)
                    TulipSession.current_offset = _msg.getId();

                if (_msg.sysType()) {
                    // TODO: System messages;
                    continue;
                }

                if (_msg.getUser().equals(TulipSession.user)) {
                    target_user_list = _msg.getFrm();
                    current_user_message = true;
                } else {
                    target_user_list = _msg.getUser();
                    current_user_message = false;
                }

                int friend_pos = friends.indexOf(target_user_list);
                if (friend_pos == -1) {
                    friends.add(target_user_list);
                    flag_update_user_list = true;
                }

                
                List<Message> _m = messages.get(target_user_list);
                if (_m == null) {
                    // this should not happen
                    // if the user is on the list but on on the client
                    messages.put(target_user_list, new ArrayList<Message>());
                    messages.get(target_user_list).add(_msg);
                } else {
                    Iterator<Message> itr = _m.iterator();
                    boolean flag_found = false;
                    while (itr.hasNext()) {
                        Message _tmsg = itr.next();
                        if (_tmsg.getId().compareTo(_msg.getId()) == 0) {
                            flag_found = true;
                            continue;
                        }
                    }
                    if (!flag_found) {
                        if(messages.get(target_user_list) != null)
                            messages.get(target_user_list).add(_msg);
                    }
                }
                

                if (current_user_message && (_msg.isNew() || flag_update_user_list)) {
                    // update UI where user is set to new message
                    
                    int pos = friends_message.indexOf(target_user_list);
                    if (pos == -1)
                        friends_message.add(target_user_list);

                    ArrayList<String> _t = new ArrayList<String>();
                    Object[] _data = MainActivity.friends.toArray();

                    for(Object o:_data){
                        _t.add(o.toString());
                    }
                    adaptor = new Userlist(this, _t);

                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            listview.invalidateViews();
                            listview.setAdapter(adaptor);
                            adaptor.notifyDataSetChanged();
                        }
                    });
                }


            }

        }
    }

    static class ChatViewHolder{
        int position;
        TextView chatuser;
        ImageView imageView;
    }

    class Userlist extends BaseAdapter {

        private final Context context;
        private ArrayList<String> data;

        public void SetData(ArrayList<String> data){

            this.data = data;
            notifyDataSetChanged();
        }

        Userlist(Context c, ArrayList<String> data)
        {
            context = c;
            this.data = data;
        }

        @Override
        public int getCount() {
            return data.size();
        }

        @Override
        public Object getItem(int i) {

            return data.get(i);
        }

        @Override
        public long getItemId(int i) {

            return i;
        }

        @Override
        public View getView(int position, View convertView, ViewGroup parent) {

            final ChatViewHolder viewHolder;

            if (convertView == null) {

                LayoutInflater inflater = (LayoutInflater) context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
                convertView  = inflater.inflate(R.layout.useritem_layout, parent, false);

                viewHolder = new ChatViewHolder();
                viewHolder.position = position;
                viewHolder.chatuser = (TextView)convertView.findViewById(R.id.usernameview);
                viewHolder.chatuser.setText(getItem(position).toString());

                viewHolder.imageView = (ImageView)convertView.findViewById(R.id.statimgview);
                int pos = friends_message.indexOf(getItem(position).toString());

                if(pos == -1)
                    viewHolder.imageView.setImageResource(stateimage[0]);
                else
                    viewHolder.imageView.setImageResource(stateimage[1]);

                convertView.setTag(viewHolder);
            }
            else {
                viewHolder= (ChatViewHolder)convertView.getTag();
                viewHolder.chatuser.setText(getItem(position).toString());
                viewHolder.position=position;
                int pos = friends_message.indexOf(getItem(position).toString());

                if(pos == -1)
                    viewHolder.imageView.setImageResource(stateimage[0]);
                else
                    viewHolder.imageView.setImageResource(stateimage[1]);
            }

            return convertView;
        }
    }

    public native String[] GetList(String user, String pass);

    public native void Logout(String user, String pass);

    public native void FriendRequest(String user, String pass, String request);

    public native String[] GetFriendRequest(String user, String pass);

    public native int AcceptFriend(String user, String pass, String freq);

    public native int IgnoreFriend(String user, String pass, String freq);

    public native Message GetMessage(String user, String pass, String frm_user, String offset);

}
