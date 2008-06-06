/**
 * 
 */
package com.ironicentertainment.core;

import java.util.List;
import java.util.Set;
import java.security.MessageDigest;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

/**
 * @author raykowski
 *
 */
public class UserServiceImpl implements UserService {
	private UserDao _userDao;
	
	/** Logger for this class and subclasses */
	protected final Log logger = LogFactory.getLog(getClass());
	
	public UserServiceImpl(UserDao dao) {
		_userDao=dao;
	}
	
	public void setUserDao(UserDao dao) {
		_userDao=dao;
	}
	
	public UserDao getUserDao() {
		return _userDao;
	}
	
	/* (non-Javadoc)
	 * @see com.ironicentertainment.core.UserService#getUser(java.lang.String)
	 */
	public User getUser(String id) {
		// TODO Auto-generated method stub
		return null;
	}

	/* (non-Javadoc)
	 * @see com.ironicentertainment.core.UserService#createUser(com.ironicentertainment.core.UserDao)
	 */
	public void createUser(UserDao user) {
		// TODO Auto-generated method stub

	}

	/* (non-Javadoc)
	 * @see com.ironicentertainment.core.UserService#getPlayers(com.ironicentertainment.core.UserDao)
	 */
	public Set getPlayers(UserDao user) {
		// TODO Auto-generated method stub
		return null;
	}

	public User isValidUser(String username, String passwd) {
		User usr = _userDao.getUserByName(username);
		logger.debug("USImpl: isValidUser: Requested name: " + username + " Username found: " + usr.getUsername() + " Pass: " + usr.getPasswd());
		
		if(usr.getPasswd().equals(passwd))
			return usr;
		else
			logger.debug("USImpl: isValidUser: Requested pass: " + passwd + " Passwd found: " + usr.getPasswd());
		
		return null;
	}
	
	public List getUserLikeName(String username) {
		List users = _userDao.getUserLikeName(username);
		return users;
	}
}
