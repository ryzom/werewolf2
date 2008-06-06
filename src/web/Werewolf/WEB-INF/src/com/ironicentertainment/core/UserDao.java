/**
 * 
 */
package com.ironicentertainment.core;

import org.springframework.dao.DataAccessException;

import java.util.List;

/**
 * @author raykowski
 *
 */
public interface UserDao {
	public User getUserById(Integer id);
	public User getUserByName(String username) throws DataAccessException;
	public List getUserLikeName(String username) throws DataAccessException;
}
