<?php
	global $topaz;
	if ($topaz->ww_users->user_validate($_GET['login'])) {
		echo '
			<h1>' . $_GET['login'] . '</h1>
			<h2>Information</h2>
			Account information here.<br>
			<br>
		';
		$user = $topaz->ww_users->user_privilege($_SESSION['username']);
		if ($user->Privilege == ':DEV:') {
			echo '<h2>Administrative tasks</h2>';
			if (isset($_GET['delete'])) {
				echo '<h3>Delete</h3>';
				if ($_GET['delete'] != 'yes') {
					echo '
						Are you sure you want to delete this account?<br>
						<a href="?login=' . $_GET['login'] . '">No</a> | <a href="?login=' . $_GET['login'] . '&amp;delete=yes">Yes</a>
					';
				} else {
					if ($topaz->ww_users->user_delete($_GET['login'])) {
						echo 'Account deleted.';
					} else {
						echo 'There was an error deleting this account.';
					}
				}
				echo '<br>';
			} else if (isset($_GET['password'])) {
				echo '<h3>Reset password</h3>';
				if (!isset($_POST['password'])) {
					echo '
						<form action="?login=' . $_GET['login'] . '&password" method="post">
							<b>New password:</b> <input type="password" name="password"> <input type="submit" value="Reset">
						</form>
					';
				} else {
					if ($topaz->ww_users->user_password($_GET['login'],$_POST['password'])) {
						echo 'Password was reset.';
					} else {
						echo 'There was an error resetting the password.';
					}
					echo '<br>';
				}
			} else {
				echo '
					<a href="?login=' . $_GET['login'] . '&amp;delete">Delete</a><br>
					<a href="?login=' . $_GET['login'] . '&amp;password">Reset password</a><br>
				';
			}
		}
	}
?>
