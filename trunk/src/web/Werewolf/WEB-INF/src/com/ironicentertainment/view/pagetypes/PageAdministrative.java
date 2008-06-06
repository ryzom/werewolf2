package com.ironicentertainment.view.pagetypes;

import org.apache.tapestry.event.PageEvent;
import org.apache.tapestry.PageRedirectException;

import com.ironicentertainment.view.Visit;
import com.ironicentertainment.view.PageError;

public class PageAdministrative extends PageAuthenticated {
	public void pageBeginRender(PageEvent event) {
		Visit visit = (Visit)getVisit();
		
		super.pageBeginRender(event);
		if(visit.getUser().checkPrivilege("ADM"))
			return;
		
		// set the error message and redirect.
		PageError page = (PageError)getRequestCycle().getPage("PageError");
		page.setMessage("You do not have administrative access.");
		page.setRedirect("Main");
		throw new PageRedirectException(page);
	}
}
