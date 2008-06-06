package com.ironicentertainment.view;

import org.apache.tapestry.html.BasePage;
import org.apache.tapestry.IRequestCycle;
import com.ironicentertainment.view.pagetypes.PageAdministrative;

public abstract class AdmUserSearch extends PageAdministrative {
	public abstract String getUsername();
	public abstract void setUsername(String username);
	
	public void formSubmit(IRequestCycle cycle) {
		AdmUserMatches matches = (AdmUserMatches) cycle.getPage("AdmUserMatches");
		
		matches.setSearchData(getUsername());
		
		cycle.activate(matches);
	}
}
