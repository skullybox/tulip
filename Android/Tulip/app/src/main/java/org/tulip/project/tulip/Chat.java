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

public class Chat extends AppCompatActivity {

    ChatList adaptor;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_chat);

        ListView lst = (ListView) findViewById(R.id.chatlistview);
        adaptor = new ChatList();
        lst.setAdapter(adaptor);
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
            return 0;
        }

        @Override
        public Object getItem(int i) {
            return null;
        }

        @Override
        public long getItemId(int i) {
            return 0;
        }

        @Override
        public View getView(int i, View view, ViewGroup viewGroup) {
            return null;
        }
    }

    public native int SendMessage(String user, String pass, String frm_user, String msg);
}

