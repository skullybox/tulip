package org.tulip.project.tulip;

import android.content.Context;
import android.content.Intent;
import android.support.design.widget.FloatingActionButton;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.*;

import java.math.BigInteger;
import java.util.ArrayList;
import java.util.Iterator;

public class Chat extends AppCompatActivity implements Runnable {

    ChatList adaptor;
    Thread _chat_view_notifier;
    ListView lst;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_chat);

        lst = (ListView) findViewById(R.id.chatlistview);
        adaptor = new ChatList(this, MainActivity.messages.get(TulipSession.current_chat_user));
        lst.setAdapter(adaptor);

        lst.setDivider(null);
        lst.setDividerHeight(0);

        addClickListener();

        _chat_view_notifier = new Thread(this);
        _chat_view_notifier.start();

        getSupportActionBar().setTitle(TulipSession.current_chat_user);




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
                final TextView chatin = (TextView) findViewById(R.id.editText7);
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
                    {
                        ret = SendMessage(TulipSession.user, TulipSession.password, TulipSession.current_chat_user,
                                msg);
                    }
                    else
                    {
                        fab.setClickable(true);
                        return;
                    }

                    if (ret == 0 && TulipSession.current_chat_user.length() > 0) {
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                chatin.setText("");
                            }
                        });

                    } else  {
                        resend++;
                    }
                }
                fab.setClickable(true);

            }
        });

    }

    @Override
    public void run() {

        int sz = 0;
        while(true)
        {
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


            if (MainActivity.messages.get(TulipSession.current_chat_user)!= null && MainActivity.messages.get(TulipSession.current_chat_user).size() != sz) {
                sz = MainActivity.messages.get(TulipSession.current_chat_user).size();
                adaptor = new ChatList(this, MainActivity.messages.get(TulipSession.current_chat_user));
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        lst.invalidateViews();
                        lst.setAdapter(adaptor);
                        adaptor.notifyDataSetChanged();
                    }
                });
            }

        }
    }

    static class ViewHolder{
        int position;
        TextView chatmsg;
    }

    class ChatList extends BaseAdapter{

        private final Context context;
        private ArrayList<Message> data;

        ChatList(Context c, ArrayList<Message> data)
        {
            context = c;
            this.data = data;
        }

        @Override
        public int getCount() {
            if(data == null)
                return 0;
            return data.size();
        }

        public Message getRawItem(int i)
        {
            return data.get(i);
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


            if(getRawItem(position).getUser().equals(TulipSession.user))
            {
                // messages received
                RelativeLayout.LayoutParams params = (RelativeLayout.LayoutParams) ((TextView) viewHolder.chatmsg).getLayoutParams();
                params.removeRule(RelativeLayout.ALIGN_PARENT_RIGHT);
                params.addRule(RelativeLayout.ALIGN_PARENT_LEFT);
                viewHolder.chatmsg.setLayoutParams(params);
                viewHolder.chatmsg.setBackgroundColor(0xff2bc5fb);
                viewHolder.chatmsg.setTextColor(0xffffffff);

            }
            else
            {
                RelativeLayout.LayoutParams params = (RelativeLayout.LayoutParams) ((TextView) viewHolder.chatmsg).getLayoutParams();
                params.removeRule(RelativeLayout.ALIGN_PARENT_LEFT);
                params.addRule(RelativeLayout.ALIGN_PARENT_RIGHT);
                viewHolder.chatmsg.setLayoutParams(params);
                viewHolder.chatmsg.setBackgroundColor(0xff768f9a);
                viewHolder.chatmsg.setTextColor(0xffffffff);
            }


            return convertView;
        }
    }

    public native int SendMessage(String user, String pass, String frm_user, String msg);
}

