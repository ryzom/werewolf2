/**
 * 
 */
package com.ironicentertainment.core;

import com.ironicentertainment.core.GreetingService;
/**
 * @author raykowski
 *
 */
public class GreetingServiceImpl implements GreetingService {
	private String greeting = "not initialized";
	
	public GreetingServiceImpl(String greeting) {
		this.greeting = greeting;
	}
	
	public String getGreeting() {
		return greeting;
	}
}
