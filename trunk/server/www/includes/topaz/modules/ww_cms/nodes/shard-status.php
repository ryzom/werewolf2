<table width="100%">
	<tr>
		<td><b>Shard</b></td>
		<td><b>Online players</b></td>
		<td><b>Status</b></td>
		<td><b>Version</b></td>
	</tr>
	<?php
		global $topaz;
		$result = $topaz->mysql->query("SELECT Name,NbPlayers,Online,Version FROM shard");
		while ($shard = $topaz->mysql->fetch_object($result)) {
			echo '
				<tr>
					<td>' . $shard->Name . '</td>
					<td>' . $shard->NbPlayers . '</td>
					<td><span style="color:' . ($shard->Status ? '#00ff00">Online' : '#ff0000">Offline') . '</span></td>
					<td>' . $shard->Version . '</td>
				</tr>
			';
		}
	?>
</table>
