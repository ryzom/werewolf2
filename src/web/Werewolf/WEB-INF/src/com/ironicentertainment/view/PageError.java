package com.ironicentertainment.view;

import org.apache.tapestry.html.BasePage;

public abstract class PageError extends BasePage {
	public abstract void setMessage(String message);
	public abstract String getMessage();
	
	public abstract String getRedirect();
	public abstract void setRedirect(String redir);
}
