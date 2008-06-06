package com.ironicentertainment.view;

import org.apache.tapestry.event.PageEvent;

import com.ironicentertainment.view.pagetypes.PageAdministrative;
import com.ironicentertainment.core.UserService;
import com.ironicentertainment.core.User;

import java.util.Iterator;
import java.util.List;

public abstract class AdmUserMatches extends PageAdministrative {
	public abstract String getSearchData();
	public abstract void setSearchData(String username);
	
	public abstract Iterator getUserMatches();
	public abstract void setUserMatches(Iterator matches);
	
	public abstract UserService getUserService();
	
	public void pageBeginRender(PageEvent event) {
		super.pageBeginRender(event);
		
		queryUsers(getSearchData());
	}
	
	public void queryUsers(String username) {
		List users = getUserService().getUserLikeName(username);
		setUserMatches(users.iterator());
	}
}
