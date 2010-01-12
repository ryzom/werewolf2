<?php
	// jimckf@jimckf.com - 2010

	class ww_users {
		// Create an account and return it's ID.
		// TODO: Validate username and password (length and content).
		function user_create($username,$password) {
			if ($this->user_validate($username)) {
				return false;
			}
			$this->parent->mysql->query("INSERT INTO user (Login,Password) VALUES ('" . $this->parent->mysql->escape(utf8_encode($username)) . "','" . md5($password) . "')");
			$user_id = $this->parent->mysql->insert_id();
			if (!$this->user_validate($username)) {
				return false;
			}
			return $user_id;
		}

		// Validate username and password, and return ID (and more?) accordingly.
		function user_validate($username,$password = null) {
			$query = "SELECT UId FROM user WHERE Login='" . $this->parent->mysql->escape(utf8_encode($username)) . "'";
			if ($password != null) {
				$query .= " AND Password='" . md5($password) . "'";
			}
			$result = $this->parent->mysql->query($query);
			if ($this->parent->mysql->num_rows($result) <= 0) {
				return false;
			}
			$user = $this->parent->mysql->fetch_object($result);
			return $user->UId;
		}

		// Start a new session.
		function session_start() {
			session_start();
			if (!isset($_SESSION['username']) || !isset($_SESSION['password'])) {
				return false; // No session in progress.
			}
			if (!$this->user_validate($_SESSION['username'],$_SESSION['password'])) {
				$this->session_destroy(); // Invalid session in progress. Destroy it.
				return false;
			}
			return true;
		}

		// Destroy the current session.
		function session_destroy() {
			unset($_SESSION);
			return session_destroy();
		}

		// Validate the account and store the details in the current session.
		function user_login($username,$password) {
			if (!$this->user_validate($username,$password)) {
				return false;
			}
			$_SESSION['username'] = $username;
			$_SESSION['password'] = $password;
			return true;
		}

		// Destroy the session to logout the user.
		function user_logout() {
			return $this->session_destroy();
		}

		// Return an array of user objects with IDs and usernames, and pssibly more.
		function user_list($fields = 'UId,Login') {
			$result = $this->parent->mysql->query("SELECT " . $this->parent->mysql->escape($fields) . " FROM user");
			while ($user = $this->parent->mysql->fetch_object($result)) {
				$users[] = $user;
			}
			return $users;
		}

		// Delete username and return a bool accordingly.
		function user_delete($username) {
			if ($this->user_validate($username)) {
				$this->parent->mysql->query("DELETE FROM user WHERE Login='" . $this->parent->mysql->escape(utf8_encode($username)) . "'");
				if ($this->user_validate($username)) {
					return false;
				}
			} else {
				return false;
			}
			return true;
		}

		// Return privileges.
		function user_privilege($username,$new_privilege = null,$new_extended = null) {
			if (!$this->user_validate($username)) {
				return false;
			}
			if ($new_privilege != null) {
				$this->parent->mysql->query("UPDATE user SET Privilege='" . $this->parent->mysql->escape($new_privilege) . "' WHERE Login='" . $this->parent->mysql->escape(utf8_encode($username)) . "'");
			}
			if ($new_extended != null) {
				$this->parent->mysql->query("UPDATE user SET ExtendedPrivilege='" . $this->parent->mysql->escape($new_extended) . "' WHERE Login='" . $this->parent->mysql->escape(utf8_encode($username)) . "'");
			}
			$result = $this->parent->mysql->query("SELECT Privilege,ExtendedPrivilege FROM user WHERE Login='" . $this->parent->mysql->escape(utf8_encode($username)) . "'");
			return $this->parent->mysql->fetch_object($result);
		}
	}
?>
