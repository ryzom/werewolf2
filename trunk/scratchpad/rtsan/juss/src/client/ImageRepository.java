/*
 * Created on May 3, 2005
 *
 */
package client;

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

	private static final Logger log = Logger.getLogger("juss.client.ImageRepository");


	static {
		images = new HashMap();
	}

	public static void loadImagesForSplashScreen(Display display) {
		loadImage(display, "client/images/tray16.ico", "tray16Icon");
		loadImage(display, "client/images/splash_logo.bmp", "splashLogo");
		loadImage(display, "client/images/gird_uus.bmp", "grid");
		loadImage(display, "client/images/emty.bmp", "empty");
		loadImage(display, "client/images/bar.bmp", "bar");
		loadImage(display, "client/images/console.bmp", "console");
		loadImage(display, "client/images/tray.bmp", "tray");
		loadImage(display, "client/images/config.bmp", "config");
		loadImage(display, "client/images/net.bmp", "net");
		loadImage(display, "client/images/window.bmp", "window");
		loadImage(display, "client/images/tray16.bmp", "tray16", 0);
	}

	public static void loadImages(Display display) {
		loadImage(display, "client/images/about.bmp", "about");
		loadImage(display, "client/images/login.bmp", "login");
		loadImage(display, "client/images/treeDirClosed.bmp", "folderClosed");
		loadImage(display, "client/images/treeDirOpen.bmp", "folderOpen");
		loadImage(display, "client/images/file.bmp", "file");
		loadImage(display, "client/images/info.bmp", "info");
		loadImage(display, "client/images/line.bmp", "line");
		loadImage(display, "client/images/main_logo.bmp", "mainLogo");
		loadImage(display, "client/images/nurk_topRight.bmp", "nurkTopRight");
		loadImage(display, "client/images/titleRight.bmp", "titleRight");
		loadImage(display, "client/images/titleLeft.bmp", "titleLeft");
		loadImage(display, "client/images/tittleMiddle.bmp", "titleMiddle");
		loadImage(display, "client/images/tray_up.bmp", "trayUp");
		loadImage(display, "client/images/close_up.bmp", "closeUp");
		loadImage(display, "client/images/menu_file.bmp", "menuFile");
		loadImage(display, "client/images/menu_fileOver.bmp", "menuFileOver");
		loadImage(display, "client/images/menu_view.bmp", "menuView");
		loadImage(display, "client/images/menu_viewOver.bmp", "menuViewOver");
		loadImage(display, "client/images/menu_options.bmp", "menuOptions");
		loadImage(display, "client/images/menu_optionsOver.bmp", "menuOptionsOver");
		loadImage(display, "client/images/menu_help.bmp", "menuHelp");
		loadImage(display, "client/images/menu_helpOver.bmp", "menuHelpOver");
		loadImage(display, "client/images/menu_end.bmp", "menuEnd");
		loadImage(display, "client/images/nurk_top.bmp", "nurkTop");
		loadImage(display, "client/images/nurk_bottom.bmp", "nurkBottom");
		loadImage(display, "client/images/syn_a.bmp", "synA");
		loadImage(display, "client/images/search_a.bmp", "searchA");
		loadImage(display, "client/images/pro_a.bmp", "proA");
		loadImage(display, "client/images/syn_b.bmp", "synB");
		loadImage(display, "client/images/search_b.bmp", "searchB");
		loadImage(display, "client/images/pro_b.bmp", "proB");
		loadImage(display, "client/images/bar_big.bmp", "barBig");
		loadImage(display, "client/images/main_nurk.bmp", "mainNurk");
		loadImage(display, "client/images/tab1.bmp", "tab1");
		loadImage(display, "client/images/tab2.bmp", "tab2");
		loadImage(display, "client/images/tab3.bmp", "tab3");
		loadImage(display, "client/images/16/connect_no.gif", "offline");
	    loadImage(display, "client/images/16/connect_established.gif", "online");
	    loadImage(display, "client/images/sizer.bmp", "sizer");
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
//					icone.alpha = alpha;
					icone.transparentPixel = alpha;
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
