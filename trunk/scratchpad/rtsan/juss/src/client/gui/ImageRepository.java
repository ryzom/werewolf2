/*
 * Created on May 3, 2005
 *
 */
package client.gui;

import java.io.InputStream;
import java.util.HashMap;
import java.util.Iterator;

import org.apache.log4j.Logger;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.ImageData;
import org.eclipse.swt.program.Program;
import org.eclipse.swt.widgets.Display;

/**
 * @author Henri Kuuste
 * 
 * Klass piltide säilitamiseks
 * 
 */
public class ImageRepository {

	private static HashMap images;
	
	private static final Logger log = Logger.getLogger("juss.client.gui.ImageRepository");

	static {
		images = new HashMap();
	}

	public static void loadImagesForSplashScreen(Display display) {
	}

	public static void loadImages(Display display) {
		loadImage(display, "client/images/login.bmp", "login");
		loadImage(display, "client/images/juss16.png", "juss16");
		loadImage(display, "client/images/juss32.png", "juss32");
		loadImage(display, "client/images/juss64.png", "juss64");
		loadImage(display, "client/images/juss128.png", "juss128");
		loadImage(display, "client/images/treeDirClosed.bmp", "folder_closed");
		loadImage(display, "client/images/treeDirOpen.bmp", "folder_open");
		loadImage(display, "client/images/file.bmp", "file");
		loadImage(display, "client/images/treeProject.bmp", "project");
		loadImage(display, "client/images/22/filenew.gif", "new");
	    loadImage(display, "client/images/22/fileopen.gif", "open");
		loadImage(display, "client/images/22/reload.gif", "refresh");
		loadImage(display, "client/images/32/view_choose.png", "tab_browser");
		loadImage(display, "client/images/32/queue.png", "tab_transfers");
		loadImage(display, "client/images/32/filefind.png", "tab_search");
		loadImage(display, "client/images/32/toggle_log.png", "tab_console");
		loadImage(display, "client/images/16/connect_no.gif", "offline");
	    loadImage(display, "client/images/16/connect_established.gif", "online");
	    loadImage(display, "client/images/16/up_completed.gif", "upload.Completed");
	    loadImage(display, "client/images/16/up_failed.gif", "upload.Failed");
	    loadImage(display, "client/images/16/up_queued.gif", "upload.Queued");
	    loadImage(display, "client/images/16/up_transfering.gif", "upload.Transfering");
	    loadImage(display, "client/images/16/up_failed.gif", "upload.Failing");
	    loadImage(display, "client/images/16/up_completed.gif", "upload.Completing");
	    loadImage(display, "client/images/16/down_completed.gif", "download.Completed");
	    loadImage(display, "client/images/16/down_failed.gif", "download.Failed");
	    loadImage(display, "client/images/16/down_queued.gif", "download.Queued");
	    loadImage(display, "client/images/16/down_transfering.gif", "download.Transfering");
	    loadImage(display, "client/images/16/down_failed.gif", "download.Failing");
	    loadImage(display, "client/images/16/down_completed.gif", "download.Completing");
	}

	public static Image loadImage(Display display, String res, String name) {
		return loadImage(display, res, name, 255);
	}

	public static Image loadImage(Display display, String res, String name,
			int alpha) {
		return loadImage(ImageRepository.class.getClassLoader(), display, res,
				name, alpha);
	}

	public static Image loadImage(ClassLoader loader, Display display,
			String res, String name, int alpha) {
		Image im = getImage(name);
		if (null == im) {
			InputStream is = loader.getResourceAsStream(res);
			if (null != is) {
				if (alpha == 255) {
					im = new Image(display, is);
				} else {
					ImageData icone = new ImageData(is);
					icone.alpha = alpha;
					im = new Image(display, icone);
				}
				images.put(name, im);
			} else {
				log.warn("ImageRepository:loadImage: Resource not found: " + res);
			}
		}
		return im;
	}

	public static void unLoadImages() {
		Iterator iter = images.values().iterator();
		while (iter.hasNext()) {
			Image im = (Image) iter.next();
			im.dispose();
		}
	}

	public static Image getImage(String name) {
		return (Image) images.get(name);
	}

	public static Image getIconFromProgram(Program program) {
		Image image = null;

		try {
			image = (Image) images.get(program);

			if (image == null) {
				if (program != null) {

					ImageData imageData = program.getImageData();
					if (imageData != null) {
						image = new Image(null, imageData, imageData
								.getTransparencyMask());
						images.put(program, image);
					}
				}
			}
		} catch (Throwable e) {
		}

		if (image == null) {
			image = (Image) images.get("folder");
		}
		return image;
	}
}
