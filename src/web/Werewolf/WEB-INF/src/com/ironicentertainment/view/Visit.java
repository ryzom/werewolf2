/**
 * 
 */
package com.ironicentertainment.view;

import java.io.Serializable;

import com.ironicentertainment.core.User;

/**
 * @author raykowski
 *
 */
public class Visit implements Serializable {
	private User _currentUser;
	public static Integer NO_SUCH_USER = new Integer(-1);
	
	public User getUser() {
		return _currentUser;
	}
	
	public void setUser(User user) {
		_currentUser=user;
		
	}
	
	public Integer getUserid() {
		if(_currentUser != null) {
			return _currentUser.getUserid();
		} else {
			return NO_SUCH_USER;
		}
	}
	
	public boolean isUserLoggedIn() {
		return _currentUser!=null;
	}
	
	public boolean isUserLoggedOut() {
		return _currentUser==null;
	}
	
	public void clearCache() {
		_currentUser=null;
	}
}
