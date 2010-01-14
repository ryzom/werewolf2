<?php
	global $topaz;
	$user = $topaz->ww_users->user_privilege($_SESSION['username']);
	if ($user->Privilege == ':DEV:') {
		echo '<table width="100%">';
		$result = $topaz->mysql->query("SELECT UId,Login,State FROM user");
		while ($user = $topaz->mysql->fetch_object($result)) {
			$nr++;
			echo '
				<tr' . ($nr & 1 ? ' class="odd"' : '') . '>
					<td><a href="' . dirname($_SERVER['PHP_SELF']) . '/user?login=' . $user->Login . '">' . $user->Login . '</a></td>
					<td><span style="color:' . ($user->State == 'Offline' ? '#ff0000">Offline' : '#00ff00">Online') . '</span></td>
				</tr>
			';
		}
		echo '</table>';
	}
?>
