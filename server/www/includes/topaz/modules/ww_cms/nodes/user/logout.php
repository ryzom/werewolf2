<?php
	global $logged_in;
	if (!$logged_in) {
		echo 'You are not logged in.';
	} else {
		global $topaz;
		$topaz->ww_users->user_logout();
		echo 'You are now logged out. Redirect here too?';
	}
?>
