/*
 * $Header: /home/cvsroot/werewolf/scratchpad/rtsan/juss/src/client/FilePart.java,v 1.1 2006/05/09 16:52:20 rtsan Exp $
 * $Revision: 1.1 $
 * $Date: 2006/05/09 16:52:20 $
 *
 * ====================================================================
 *
 *  Copyright 2002-2004 The Apache Software Foundation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 * ====================================================================
 *
 * This software consists of voluntary contributions made by many
 * individuals on behalf of the Apache Software Foundation.  For more
 * information on the Apache Software Foundation, please see
 * <http://www.apache.org/>.
 *
 */

package client;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import org.apache.commons.httpclient.methods.multipart.FilePartSource;
import org.apache.commons.httpclient.methods.multipart.Part;
import org.apache.commons.httpclient.methods.multipart.PartBase;
import org.apache.commons.httpclient.methods.multipart.PartSource;
import org.apache.commons.httpclient.util.EncodingUtil;
import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import shared.IProgress;

/**
 * This class implements a part of a Multipart post object that
 * consists of a file.
 * Modified to use a progress listener  
 *
 * @author <a href="mailto:mattalbright@yahoo.com">Matthew Albright</a>
 * @author <a href="mailto:jsdever@apache.org">Jeff Dever</a>
 * @author <a href="mailto:adrian@ephox.com">Adrian Sutton</a>
 * @author <a href="mailto:becke@u.washington.edu">Michael Becke</a>
 * @author <a href="mailto:mdiggory@latte.harvard.edu">Mark Diggory</a>
 * @author <a href="mailto:mbowler@GargoyleSoftware.com">Mike Bowler</a>
 * @author <a href="mailto:oleg@ural.ru">Oleg Kalnichevski</a>
 * @author <a href="mailto:rtsan@hot.ee">Henri Kuuste</a>
 *   
 * @since 2.0 
 *
 */
public class FilePart extends PartBase {

    /** Default content encoding of file attachments. */
    public static final String DEFAULT_CONTENT_TYPE = "application/octet-stream";

    /** Default charset of file attachments. */
    public static final String DEFAULT_CHARSET = "ISO-8859-1";

    /** Default transfer encoding of file attachments. */
    public static final String DEFAULT_TRANSFER_ENCODING = "binary";

    /** Log object for this class. */
    private static final Log LOG = LogFactory.getLog(FilePart.class);

    /** Attachment's file name */
    protected static final String FILE_NAME = "; filename=";

    /** Attachment's file name as a byte array */
    protected static final byte[] FILE_NAME_BYTES = 
        EncodingUtil.getAsciiBytes(FILE_NAME);

	/**
	 * Source of the file part.
	 * 
	 * @uml.property name="source"
	 * @uml.associationEnd multiplicity="(1 1)"
	 */
	private PartSource source;

	/**
	 * List of progress listeners
	 * 
	 * @uml.property name="progressListeners"
	 * @uml.associationEnd elementType="shared.IProgress" multiplicity="(0 -1)"
	 */
	private List progressListeners;


    /**
     * FilePart Constructor.
     *
     * @param name the name for this part
     * @param partSource the source for this part
     * @param contentType the content type for this part, if <code>null</code> the 
     * {@link #DEFAULT_CONTENT_TYPE default} is used
     * @param charset the charset encoding for this part, if <code>null</code> the 
     * {@link #DEFAULT_CHARSET default} is used
     */
    public FilePart(String name, PartSource partSource, String contentType, String charset) {
        
        super(
            name, 
            contentType == null ? DEFAULT_CONTENT_TYPE : contentType, 
            charset == null ? "ISO-8859-1" : charset, 
            DEFAULT_TRANSFER_ENCODING
        );

        if (partSource == null) {
            throw new IllegalArgumentException("Source may not be null");
        }
        if (partSource.getLength() < 0) {
            throw new IllegalArgumentException("Source length must be >= 0");
        }
        this.source = partSource;
        progressListeners = new ArrayList();
    }
        
    /**
     * FilePart Constructor.
     *
     * @param name the name for this part
     * @param partSource the source for this part
     */
    public FilePart(String name, PartSource partSource) {
        this(name, partSource, null, null);
    }

    /**
     * FilePart Constructor.
     *
     * @param name the name of the file part
     * @param file the file to post
     *
     * @throws FileNotFoundException if the <i>file</i> is not a normal
     * file or if it is not readable.
     */
    public FilePart(String name, File file) 
    throws FileNotFoundException {
        this(name, new FilePartSource(file), null, null);
    }

    /**
     * FilePart Constructor.
     *
     * @param name the name of the file part
     * @param file the file to post
     * @param contentType the content type for this part, if <code>null</code> the 
     * {@link #DEFAULT_CONTENT_TYPE default} is used
     * @param charset the charset encoding for this part, if <code>null</code> the 
     * {@link #DEFAULT_CHARSET default} is used
     *
     * @throws FileNotFoundException if the <i>file</i> is not a normal
     * file or if it is not readable.
     */
    public FilePart(String name, File file, String contentType, String charset) 
    throws FileNotFoundException {
        this(name, new FilePartSource(file), contentType, charset);
    }

     /**
     * FilePart Constructor.
     *
     * @param name the name of the file part
     * @param fileName the file name 
     * @param file the file to post
     *
     * @throws FileNotFoundException if the <i>file</i> is not a normal
     * file or if it is not readable.
     */
    public FilePart(String name, String fileName, File file) 
    throws FileNotFoundException {
        this(name, new FilePartSource(fileName, file), null, null);
    }
    
     /**
     * FilePart Constructor.
     *
     * @param name the name of the file part
     * @param fileName the file name 
     * @param file the file to post
     * @param contentType the content type for this part, if <code>null</code> the 
     * {@link #DEFAULT_CONTENT_TYPE default} is used
     * @param charset the charset encoding for this part, if <code>null</code> the 
     * {@link #DEFAULT_CHARSET default} is used
     *
     * @throws FileNotFoundException if the <i>file</i> is not a normal
     * file or if it is not readable.
     */
    public FilePart(String name, String fileName, File file, String contentType, String charset) 
    throws FileNotFoundException {
        this(name, new FilePartSource(fileName, file), contentType, charset);
    }
    
    /**
     * Write the disposition header to the output stream
     * @param out The output stream
     * @throws IOException If an IO problem occurs
     * @see Part#sendDispositionHeader(OutputStream)
     */
    protected void sendDispositionHeader(OutputStream out) 
    throws IOException {
        LOG.trace("enter sendDispositionHeader(OutputStream out)");
        super.sendDispositionHeader(out);
        String filename = this.source.getFileName();
        if (filename != null) {
            out.write(FILE_NAME_BYTES);
            out.write(QUOTE_BYTES);
            out.write(EncodingUtil.getAsciiBytes(filename));
            out.write(QUOTE_BYTES);
        }
    }
    
    /**
     * Write the data in "source" to the specified stream.
     * @param out The output stream.
     * @throws IOException if an IO problem occurs.
     * @see org.apache.commons.httpclient.methods.multipart.Part#sendData(OutputStream)
     */
    protected void sendData(OutputStream out) throws IOException {
        LOG.trace("enter sendData(OutputStream out)");
        if (lengthOfData() == 0) {
            
            // this file contains no data, so there is nothing to send.
            // we don't want to create a zero length buffer as this will
            // cause an infinite loop when reading.
            LOG.debug("No data to send.");
            return;
        }
        
        byte[] tmp = new byte[4096];
        InputStream instream = source.createInputStream();
        try {
            int len;
            while ((len = instream.read(tmp)) >= 0) {
                out.write(tmp, 0, len);
                Iterator it = progressListeners.iterator();
                while(it.hasNext()) {
                	((IProgress)it.next()).addDone(len);
                }
            }
        } finally {
            // we're done with the stream, close it
            instream.close();
        }
    }

	/**
	 * Returns the source of the file part.
	 *  
	 * @return The source.
	 * 
	 * @uml.property name="source"
	 */
	protected PartSource getSource() {
		LOG.trace("enter getSource()");
		return this.source;
	}

    /**
     * Return the length of the data.
     * @return The length.
     * @throws IOException if an IO problem occurs
     * @see org.apache.commons.httpclient.methods.multipart.Part#lengthOfData()
     */    
    protected long lengthOfData() throws IOException {
        LOG.trace("enter lengthOfData()");
        return source.getLength();
    }    
    
    /**
     * Add a progress listener
     * @param p The listener to add
     */
    public void addProgressListener(IProgress p) {
    	progressListeners.add(p);
    }
    
    /**
     * Remove a progress listener
     * @param p The listener to remove
     */    
    public void removeProgressListener(IProgress p) {
    	progressListeners.remove(p);
    }

}
