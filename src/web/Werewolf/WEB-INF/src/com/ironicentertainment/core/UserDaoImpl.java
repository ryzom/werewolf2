/**
 * 
 */
package com.ironicentertainment.core;

//import org.springframework.orm.hibernate.HibernateTemplate;
import org.springframework.orm.hibernate3.support.HibernateDaoSupport;
import org.springframework.dao.DataAccessException;

import java.util.List;

/**
 * @author raykowski
 *
 */
public class UserDaoImpl extends HibernateDaoSupport implements UserDao  {
	
	/* (non-Javadoc)
	 * @see com.ironicentertainment.core.UserDao#getUserById(java.lang.Integer)
	 */
	public User getUserById(Integer id) {
		// TODO Auto-generated method stub
		return null;
	}

	public User getUserByName(String username) throws DataAccessException {
		logger.debug("Before select query...");
		List listusers = getHibernateTemplate().find("from User users where users.username=?", username);
		
		if(listusers.size() > 1) {
			logger.error("USER LOOKUP: Too many users matched query for username: " + username);
		}
		
		User usr = (User)listusers.get(0);
		logger.debug("USER LOOKUP: local: " + username + " dao: " + usr.getUsername() + " pass: " + usr.getPasswd());
		return usr;
	}
	
	public List getUserLikeName(String username) throws DataAccessException {
		logger.debug("Before select query...");
		List listusers = getHibernateTemplate().find("from User users where users.username like '" + username + "%'");
		return listusers;
	}
}
