package com.ironicentertainment.view.pagetypes;

import org.apache.tapestry.html.BasePage;
import org.apache.tapestry.event.PageRenderListener;
import org.apache.tapestry.event.PageEvent;
import org.apache.tapestry.PageRedirectException;

import com.ironicentertainment.view.Visit;

public class PageAuthenticated extends BasePage implements PageRenderListener {
	public void pageBeginRender(PageEvent event) {
		Visit user = (Visit)getVisit();
		
		if(user != null && user.isUserLoggedIn())
			if(user.getUser().checkPrivilege("USR"))
				return;
		
		throw new PageRedirectException("Login");
		//getRequestCycle().activate("Login");
	}
	
	public void pageEndRender(PageEvent event) {
		return;
	}
}
