package org.tulip.project.tulip;

import android.content.Context;
import android.content.Intent;
import android.support.design.widget.FloatingActionButton;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.BaseAdapter;
import android.widget.ListView;
import android.widget.TextView;

import java.math.BigInteger;
import java.util.ArrayList;
import java.util.Iterator;

public class Chat extends AppCompatActivity implements Runnable {

    ChatList adaptor;
    Thread _chat_view_notifier;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_chat);

        ListView lst = (ListView) findViewById(R.id.chatlistview);
        adaptor = new ChatList(this, MainActivity.messages.get(TulipSession.current_chat_user));
        lst.setAdapter(adaptor);

        lst.setDivider(null);
        lst.setDividerHeight(0);

        addClickListener();

        _chat_view_notifier = new Thread(this);
        _chat_view_notifier.start();

    }

    @Override
    public void onBackPressed() {
        super.onBackPressed();
        _chat_view_notifier.interrupt();
    }

    private void addClickListener()
    {
        final FloatingActionButton fab = (FloatingActionButton) findViewById(R.id.sendbutton);

        fab.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                fab.setClickable(false);
                int resend = 0;
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

                int ret = -1;

                while(ret != 0 && resend < 2) {
                    if (_m.getMessage().length() > 0)
                        ret = SendMessage(TulipSession.user, TulipSession.password, TulipSession.current_chat_user,
                                msg);
                    else
                        ret = -1;

                    if (ret == 0 && TulipSession.current_chat_user.length() > 0) {
                        chatin.setText("");
                    } else if (resend == 0) {
                        resend++;
                    }
                }
                fab.setClickable(true);

            }
        });

    }

    @Override
    public void run() {

        while(true)
        {
            int sz = 0;

            sz = MainActivity.messages.get(TulipSession.current_chat_user).size();
            try {
                Thread.sleep(500);
                if(_chat_view_notifier.isInterrupted())
                    break;
                Thread.sleep(500);
                if(_chat_view_notifier.isInterrupted())
                    break;

            } catch (InterruptedException e) {
                break;
            }

            if (MainActivity.messages.get(TulipSession.current_chat_user).size() != sz)
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        adaptor.notifyDataSetChanged();
                    }
                });

        }
    }

    static class ViewHolder{
        int position;
        TextView chatmsg;
    }

    class ChatList extends BaseAdapter{

        private final Context context;
        private final ArrayList<Message> data;

        ChatList(Context c, ArrayList<Message> data)
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

            return data.get(i).getMessage();
        }

        @Override
        public long getItemId(int i) {

            return i;
        }

        @Override
        public View getView(int position, View convertView, ViewGroup parent) {

            final ViewHolder viewHolder;

            if (convertView == null) {

                LayoutInflater inflater = (LayoutInflater) context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
                convertView  = inflater.inflate(R.layout.chatitem_layout, parent, false);

                viewHolder = new ViewHolder();
                viewHolder.position = position;
                viewHolder.chatmsg = (TextView)convertView.findViewById(R.id.chatcontent);
                viewHolder.chatmsg.setText(getItem(position).toString());
                convertView.setTag(viewHolder);
            }
            else {
                viewHolder= (ViewHolder)convertView.getTag();
                viewHolder.chatmsg.setText(getItem(position).toString());
                viewHolder.position=position;
            }

            return convertView;
        }
    }

    public native int SendMessage(String user, String pass, String frm_user, String msg);
}

