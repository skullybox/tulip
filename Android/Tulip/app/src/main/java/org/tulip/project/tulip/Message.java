package org.tulip.project.tulip;

import java.math.BigInteger;

/**
 * Created by skully on 20/11/17.
 */

public class Message {

    private BigInteger message_id = BigInteger.ZERO;
    private String message = "";
    private String user = "";
    private String frm = "";
    private boolean new_msg = false;
    private String type = "";

    Message(String message, String user, String frm, BigInteger message_id, boolean new_msg, String type)
    {
        this.user = user;
        this.frm = frm;
        this.new_msg = new_msg;
        this.message_id = message_id;
        this.message = message;
        this.type = type;
    }

    public boolean sysType(){
        if(type.equals("SYS"))
            return true;
        else
            return false;
    }

    public String getMessage(){
        return message;
    }

    public void setRead(boolean new_msg)
    {
        this.new_msg = new_msg;
    }

    public BigInteger getId(){
        return message_id;
    }

    public String getUser()
    {
        return user;
    }
    public String getFrm() { return frm; }

    public boolean isNew(){
        return new_msg;
    }

}
