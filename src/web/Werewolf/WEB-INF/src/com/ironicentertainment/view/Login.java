package com.ironicentertainment.view;

import org.apache.tapestry.html.BasePage;
import org.apache.tapestry.IRequestCycle;
import com.ironicentertainment.core.UserService;
import com.ironicentertainment.core.User;

public abstract class Login extends BasePage {
	public abstract String getUserName();
	public abstract String getPassword();
	public abstract UserService getUserService();
	
	public abstract void setMessage(String message);
	
	public void formSubmit(IRequestCycle cycle) {
		if(isValidLogin(getUserName(), getPassword())) {
			cycle.activate("Main");
			return;
		}
		
		setMessage("Invalid username or password.");
	}
	
	private boolean isValidLogin(String username, String password) {
		User user = getUserService().isValidUser(username,password);
		if(user == null) {
			return false;
		} else {
			Visit currentVisitor = (Visit)getVisit();
			currentVisitor.setUser(user);
			return true;
		}
	}
}
