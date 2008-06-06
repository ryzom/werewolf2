/**
 * 
 */
package com.ironicentertainment.view;

import java.util.Map;

import org.apache.tapestry.engine.BaseEngine;
import org.apache.tapestry.request.RequestContext;
import org.springframework.context.ApplicationContext;
import org.springframework.web.context.support.WebApplicationContextUtils;

/**
 * @author Matt Raykowski
 *
 */
public class WerewolfEngine extends BaseEngine {
	public static final String APPLICATION_CONTEXT_KEY = "appContext";
	
	protected void setupForRequest(RequestContext context) {
		super.setupForRequest(context);
		
		Map global = (Map) getGlobal();
		ApplicationContext ac = (ApplicationContext) global.get(APPLICATION_CONTEXT_KEY);
		if(ac == null) {
			ac = WebApplicationContextUtils.getWebApplicationContext(context.getServlet().getServletContext());
			global.put(APPLICATION_CONTEXT_KEY, ac);
		}
	}
}
