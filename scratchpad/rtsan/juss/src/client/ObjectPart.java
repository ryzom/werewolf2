/*
 * Created on Apr 20, 2005
 *
 *
 * Java Unified Sharing System (JUSS)
 * Copyright (c)2005 Henri Kuuste
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

package client;

import java.io.*;

import org.apache.commons.httpclient.methods.multipart.PartBase;
import org.apache.commons.httpclient.util.EncodingUtil;

/**
 * @author Henri Kuuste
 *
 */
public class ObjectPart extends PartBase {
 
	/** Default content encoding of file attachments. */
    public static final String DEFAULT_CONTENT_TYPE = "application/octet-stream";

    /** Default charset of file attachments. */
    public static final String DEFAULT_CHARSET = "ISO-8859-1";

    /** Default transfer encoding of file attachments. */
    public static final String DEFAULT_TRANSFER_ENCODING = "binary";
    
    /** Attachment's file name */
    protected static final String FILE_NAME = "; filename=";

    /** Attachment's file name as a byte array */
    protected static final byte[] FILE_NAME_BYTES = 
        EncodingUtil.getAsciiBytes(FILE_NAME);
    
    /**
     * Object to be transfered by this class
     */
    private Object obj;

    /**
     * Creates a new part that transfers objects via a multipart form in HTTP
     * @param o The object to be transfered
     */
    public ObjectPart(Object o) {
    	super("object", DEFAULT_CONTENT_TYPE, DEFAULT_CHARSET, DEFAULT_TRANSFER_ENCODING);
    	obj = o;
    }

    /**
     * Send the HTTP part header to the stream
     */
    protected void sendDispositionHeader(OutputStream out) 
    throws IOException {
        super.sendDispositionHeader(out);
        out.write(FILE_NAME_BYTES);
        out.write(QUOTE_BYTES);
        out.write(EncodingUtil.getAsciiBytes("object"));
        out.write(QUOTE_BYTES);
    }
    
	/**
	 * Send the object data to the stream
	 */
	protected void sendData(OutputStream out) throws IOException {
		ObjectOutputStream os = new ObjectOutputStream(out);
		try {
			os.writeObject(obj);
		} catch(Exception e) {
		}
	}

	/**
	 * Returns the length of the data to be sent of the web.
	 * This method may be a little slow at the moment.
	 * TODO: Make this faster if possible. Memoizing maybe?
	 */
	protected long lengthOfData() throws IOException {
	    try{
	        ByteArrayOutputStream baos = new ByteArrayOutputStream();
	        ObjectOutputStream oos = new ObjectOutputStream(baos);
	        oos.writeObject(obj);
	        byte[] bytes = baos.toByteArray();
	        oos.close();
	        baos.close();
	        return bytes.length;
	      }
	      catch(Exception e){
	        e.printStackTrace();
	      }
	      return 0;
	}

}
