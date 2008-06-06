/**
 * 
 */
package com.ironicentertainment.core;

import java.util.List;

/**
 * @author raykowski
 *
 */
public interface UserService {
	public User getUser(String id);
	public void createUser(UserDao user);
	public java.util.Set getPlayers(UserDao user);
	
	public UserDao getUserDao();
	public void setUserDao(UserDao dao);
	
	public User isValidUser(String username, String passwd);
	public List getUserLikeName(String username);
}
