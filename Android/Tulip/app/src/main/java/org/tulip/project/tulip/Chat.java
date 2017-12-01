package org.tulip.project.tulip;

import android.support.design.widget.FloatingActionButton;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.view.ViewGroup;
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

//        lst.setDivider(null);
//        lst.setDividerHeight(0);

        addClickListener();
    }


    private void addClickListener()
    {
        FloatingActionButton fab = (FloatingActionButton) findViewById(R.id.sendbutton);

        fab.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                TextView chatin = (TextView) findViewById(R.id.editText7);
                String msg = new StringBuilder(chatin.getText()).toString();

                Message _m = new Message( msg, TulipSession.user, TulipSession.current_chat_user, new BigInteger("-1"),
                        false, "SYS");

                ArrayList<Message> chatlist = MainActivity.messages.get(TulipSession.current_chat_user);
                if(chatlist == null)
                {
                    MainActivity.messages.put(TulipSession.current_chat_user, new ArrayList<Message>());
                    chatlist = MainActivity.messages.get(TulipSession.current_chat_user);
                }
                if(chatlist == null)
                    return;

                int ret = 0;
                if(msg.length() > 0)
                    SendMessage(TulipSession.user, TulipSession.password, TulipSession.current_chat_user,
                        msg);

                if(ret == 0)
                {
                    chatin.setText("");
                    chatlist.add(_m);
                    adaptor.notifyDataSetChanged();
                }

            }
        });

    }

    class ChatList extends BaseAdapter{

        @Override
        public int getCount() {
            ArrayList<Message> _msgs = MainActivity.messages.get(TulipSession.current_chat_user);
            if(_msgs == null)
                return 0;
            return _msgs.size();
        }

        @Override
        public Object getItem(int i) {

            ArrayList<Message> _msgs = MainActivity.messages.get(TulipSession.current_chat_user);
            if(_msgs == null)
                return null;

            return _msgs.listIterator(i).next();
        }

        @Override
        public long getItemId(int i) {

            ArrayList<Message> _msgs = MainActivity.messages.get(TulipSession.current_chat_user);
            if(_msgs == null)
                return 0;
            return _msgs.listIterator(i).nextIndex();
        }

        @Override
        public View getView(int i, View view, ViewGroup viewGroup) {

            view = getLayoutInflater().inflate(R.layout.activity_chat, null);
            TextView textView = (TextView) view.findViewById(R.id.chattext);

            int count = 0;
            Message current_message = null;
            ArrayList<Message> _msgs = MainActivity.messages.get(TulipSession.current_chat_user);

            if(_msgs == null)
                return null;

            Iterator<Message> itr = _msgs.iterator();
            while(itr.hasNext() && count <= i)
            {
                if(count == i) {
                    current_message = itr.next();
                    break;
                }
                else
                {
                    itr.next();
                }
                count++;
            }

            if (count == i)
            {
                if(current_message.getFrm().equals(TulipSession.user))
                {
                    textView.setTextColor(android.R.color.black);
                    textView.setBackgroundColor(android.R.color.darker_gray);
                }
                else
                {
                    textView.setTextColor(android.R.color.white);
                    textView.setBackgroundColor(android.R.color.holo_blue_light);
                }
                textView.setText(current_message.getMessage());

                return textView;
            }

            return null;
        }
    }

    public native int SendMessage(String user, String pass, String frm_user, String msg);
}

