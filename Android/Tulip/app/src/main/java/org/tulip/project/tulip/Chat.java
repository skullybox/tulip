package org.tulip.project.tulip;

import android.content.Intent;
import android.support.design.widget.FloatingActionButton;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.BaseAdapter;
import android.widget.ListView;
import android.widget.TextView;

import java.math.BigInteger;
import java.util.ArrayList;
import java.util.Iterator;

public class Chat extends AppCompatActivity {

    ChatList adaptor;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_chat);

        ListView lst = (ListView) findViewById(R.id.chatlistview);
        adaptor = new ChatList();
        lst.setAdapter(adaptor);

        lst.setDivider(null);
        lst.setDividerHeight(0);

        addClickListener();
    }

    private void addClickListener()
    {
        final FloatingActionButton fab = (FloatingActionButton) findViewById(R.id.sendbutton);

        fab.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                fab.setClickable(false);
                TextView chatin = (TextView) findViewById(R.id.editText7);
                String msg = new StringBuilder(chatin.getText()).toString();

                Message _m = new Message( msg, TulipSession.user, TulipSession.current_chat_user, new BigInteger("-1"),
                        false, "SYS");

                MainActivity.lck.lock();
                ArrayList<Message> chatlist = MainActivity.messages.get(TulipSession.current_chat_user);
                if(chatlist == null)
                {
                    MainActivity.messages.put(TulipSession.current_chat_user, new ArrayList<Message>());
                    chatlist = MainActivity.messages.get(TulipSession.current_chat_user);
                }
                MainActivity.lck.unlock();

                if(chatlist == null)
                    return;

                int ret = 0;
                if(_m.getMessage().length() > 0)
                    ret = SendMessage(TulipSession.user, TulipSession.password, TulipSession.current_chat_user,
                        msg);
                else
                    ret = -1;

                if(ret == 0 && TulipSession.current_chat_user.length() > 0)
                {
                    chatin.setText("");
                    if(_m.getMessage().length()>0)
                        chatlist.add(_m);

                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            adaptor.notifyDataSetChanged();
                        }
                    });
                }
                fab.setClickable(true);

            }
        });

        ListView userList = (ListView) findViewById(R.id.chatlistview);
        userList.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> adapterView, View view, int i, long l) {
                return;
            }
        });

    }

    class ChatList extends BaseAdapter{

        @Override
        public int getCount() {

            MainActivity.lck.lock();
            ArrayList<Message> _msgs = MainActivity.messages.get(TulipSession.current_chat_user);
            MainActivity.lck.unlock();
            if(_msgs == null)
                return 0;
            return _msgs.size();
        }

        @Override
        public Object getItem(int i) {

            MainActivity.lck.lock();
            ArrayList<Message> _msgs = MainActivity.messages.get(TulipSession.current_chat_user);
            MainActivity.lck.unlock();
            if(_msgs == null)
                return null;

            return _msgs.listIterator(i).next();
        }

        @Override
        public long getItemId(int i) {

            MainActivity.lck.lock();
            ArrayList<Message> _msgs = MainActivity.messages.get(TulipSession.current_chat_user);
            MainActivity.lck.unlock();
            if(_msgs == null)
                return 0;
            return _msgs.listIterator(i).next().getMessage().hashCode();
        }

        @Override
        public View getView(int i, View view, ViewGroup viewGroup) {

            view = getLayoutInflater().inflate(R.layout.chatitem_layout, null);
            TextView textView = (TextView) view.findViewById(R.id.chatcontent);

            Message current_message = null;
            MainActivity.lck.lock();
            ArrayList<Message> _msgs = MainActivity.messages.get(TulipSession.current_chat_user);
            MainActivity.lck.unlock();
            if(_msgs == null)
                return view;

            current_message = _msgs.listIterator(i).next();

            if (current_message != null)
            {
//                if(current_message.getFrm().equals(TulipSession.user))
//                {
//                    textView.setTextColor(android.R.color.black);
//                    textView.setBackgroundColor(android.R.color.darker_gray);
//                }
//                else
//                {
//                    textView.setTextColor(android.R.color.white);
//                    textView.setBackgroundColor(android.R.color.holo_blue_light);
//                }

                textView.setText(current_message.getMessage());
            }

            return view;
        }
    }

    public native int SendMessage(String user, String pass, String frm_user, String msg);
}

