package org.tulip.project.tulip;

import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.support.design.widget.FloatingActionButton;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.text.InputType;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.*;

import java.math.BigInteger;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;

public class MainActivity extends AppCompatActivity implements Runnable {

    Userlist adaptor;
    Thread _main_tulip;
    String[] friendRequestList = null;
    public static List<String> friends = new ArrayList<>();
    public static List<String> friends_message = new ArrayList<>(); // list of users with new messages
    public static List<String> friendsRequest = new ArrayList<>();
    int[] stateimage = {R.drawable.grey, R.drawable.pink};
    public static HashMap<String, ArrayList<Message>> messages = new HashMap<>();

    AlertDialog.Builder freqDialog;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        freqDialog = new AlertDialog.Builder(this);

        ListView listview = (ListView) findViewById(R.id.listView);
        FloatingActionButton fab = (FloatingActionButton) findViewById(R.id.floatingActionButton2);

        fab.setVisibility(View.INVISIBLE);

        String _friends[] = GetList(TulipSession.user, TulipSession.password);

        for (String _s : _friends)
            friends.add(_s);

        adaptor = new Userlist();
        listview.setAdapter(adaptor);

        // fab click listener
        addClickListener();

        _main_tulip = new Thread(this);
        _main_tulip.start();

    }
    private void addClickListener()
    {
        FloatingActionButton fab = (FloatingActionButton) findViewById(R.id.floatingActionButton2);
        fab.setOnClickListener(new View.OnClickListener(){

            @Override
            public void onClick(View view) {

                freqDialog.setTitle("Friend Request: " + friendsRequest.get(0));

                freqDialog.setPositiveButton("Accept", new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        FloatingActionButton fab = (FloatingActionButton) findViewById(R.id.floatingActionButton2);
                        int ret = AcceptFriend(TulipSession.user, TulipSession.password,
                                friendsRequest.get(0));
                        if(ret == 0) {
                            friends.add(friendsRequest.get(0));

                            adaptor.notifyDataSetChanged();

                            friendsRequest.remove(0);
                        }
                        if(friendsRequest.size() == 0)
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
                                friendsRequest.get(0));
                        friendsRequest.remove(0);
                        if(friendsRequest.size() == 0)
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
                startActivity(new Intent(MainActivity.this, Chat.class));
            }
        });

    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
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
                Logout(TulipSession.user, TulipSession.password);
                TulipSession.user = "";
                TulipSession.password = "";
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
        ListView listview = (ListView) findViewById(R.id.listView);
        final FloatingActionButton fab = (FloatingActionButton) findViewById(R.id.floatingActionButton2);

        int indexRet = 0;

        long friend_request_last_check = System.currentTimeMillis() / 1000L - 20;
        long message_last_check = System.currentTimeMillis() / 1000L - 10;

        while (true) {

            try {
                Thread.sleep(1000);
            } catch (InterruptedException e) {
                break;
            }

            // check for friend requests
            if (System.currentTimeMillis() / 1000L - friend_request_last_check > 17)
                if (friendRequestList == null || friendRequestList.length == 0) {

                    friendRequestList = GetFriendRequest(TulipSession.user, TulipSession.password);

                    if (friendRequestList != null && friendRequestList.length > 0)
                    {
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                // This code will always run on the UI thread, therefore is safe to modify UI elements.
                                fab.setVisibility(View.VISIBLE);
                            }
                        });

                    }
                    else
                    {
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                // This code will always run on the UI thread, therefore is safe to modify UI elements.
                                fab.setVisibility(View.INVISIBLE);
                            }
                        });
                    }

                    for(String _f : friendRequestList)
                    {
                        friendsRequest.add(_f);
                    }

                    friend_request_last_check = System.currentTimeMillis() / 1000L;
                }

            // check for friends messages
            if (System.currentTimeMillis() / 1000L - message_last_check > 7) {

                message_last_check = System.currentTimeMillis() / 1000L;

                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {

                        String target_user_list = "";
                        boolean flag_update_user_list = false;
                        boolean current_user_message = false;
                        Message _msg = GetMessage(TulipSession.user, TulipSession.password, "",
                                TulipSession.current_offset);

                        if(_msg.getMessage().length() <= 0)
                            return;

                        if(_msg.getId().compareTo(TulipSession.current_offset) == 1)
                            TulipSession.current_offset = _msg.getId();

                        if(_msg.sysType())
                        {
                            // TODO: System messages;
                            return;
                        }

                        if(_msg.getFrm().equals(TulipSession.user)) {
                            target_user_list = _msg.getUser();
                            current_user_message = true;
                        }
                        else {
                            target_user_list = _msg.getFrm();
                        }

                        int friend_pos = friends.indexOf(target_user_list);
                        if(friend_pos == -1 )
                        {
                            friends.add(target_user_list);
                            flag_update_user_list = true;
                        }

                        List<Message> _m = messages.get(_msg.getUser());
                        if(_m == null)
                        {
                            // this should not happen
                            // if the user is on the list but on on the client
                            messages.put(target_user_list, new ArrayList<Message>());
                            messages.get(target_user_list).add(_msg);
                        }
                        else
                        {
                            Iterator<Message> itr = _m.iterator();
                            boolean flag_found = false;
                            while(itr.hasNext())
                            {
                                Message _tmsg = itr.next();
                                if(_tmsg.getId().compareTo(_msg.getId()) == 0)
                                {
                                    flag_found = true;
                                    break;
                                }
                            }
                            if(!flag_found)
                                messages.get(target_user_list).add(_msg);
                        }

                        if(!current_user_message && (_msg.isNew()||flag_update_user_list))
                        {
                            // update UI where user is set to new message
                            int pos = friends_message.indexOf(target_user_list);
                            if(pos == -1)
                                friends_message.add(target_user_list);

                            adaptor.notifyDataSetChanged();
                        }


                    }
                });
            }

        }
    }

    class Userlist extends BaseAdapter {

        @Override
        public int getCount() {

            return friends.size();
        }

        @Override
        public Object getItem(int i) {
            return friends.listIterator(i).next();
        }

        @Override
        public long getItemId(int i) {
            return friends.listIterator(i).nextIndex();
        }

        @Override
        public View getView(int i, View view, ViewGroup viewGroup) {

            view = getLayoutInflater().inflate(R.layout.useritem_layout, null);
            ImageView imageView = (ImageView) view.findViewById(R.id.statimgview);
            TextView textView = (TextView) view.findViewById(R.id.usernameview);

            int pos = friends_message.indexOf(getItem(i).toString());
            if(pos == -1)
                imageView.setImageResource(stateimage[0]);
            else
                imageView.setImageResource(stateimage[1]);

            textView.setText(getItem(i).toString());

            return view;
        }
    }

    public native String[] GetList(String user, String pass);
    public native void Logout(String user, String pass);
    public native void FriendRequest(String user, String pass, String request);
    public native String[] GetFriendRequest(String user, String pass);
    public native int AcceptFriend(String user, String pass, String freq);
    public native int IgnoreFriend(String user, String pass, String freq);
    public native Message GetMessage(String user, String pass, String frm_user, BigInteger offset);

}
