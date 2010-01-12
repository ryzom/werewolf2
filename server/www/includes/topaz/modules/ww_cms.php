<?php
	class ww_cms {
		// Create a new menu item.
		function menu_create($parent,$title,$url) {
			return $this->parent->mysql->query("INSERT INTO cms_menu (ParentID,Title,URL) VALUES ('" . $this->parent->mysql->escape($parent) . "','" . $this->parent->mysql->escape($title) . "','" . $this->parent->mysql->escape($url) . "')");
		}

		// Load a menu item and all its children.
		// TODO: Maybe load the whole table in one query instead of multiple?
		function menu_load($parent_id = null) {
			$query = "SELECT * FROM cms_menu WHERE ParentID";
			if ($parent_id == null) {
				$query .= " IS NULL";
			} else {
				$query .= "='" . $parent_id . "'";
			}
			$query .= " ORDER BY Weight";
			$result = $this->parent->mysql->query($query);
			while ($item = $this->parent->mysql->fetch_object($result)) {
				$children = $this->menu_load($item->ID);
				if (sizeof($children) != 0) {
					$item->children = $children;
				}
				$menu[] = $item;
			}
			return $menu;
		}

		// Print a menu item and all its children, or all menus.
		function menu_print($path = null,$items = null) {
			if ($items == null) {
				$items = $this->menu_load();
				echo '<ul>';
				$parent = true;
			}
			foreach ($items as $item) {
				echo '<li>';
				if ($item->Title == '---') {
					echo '<br>';
				} else {
					echo '<a';
					if ($item->URL == $path) {
						echo ' class="active"';
					}
					echo ' href="' . dirname($_SERVER['PHP_SELF']) . '/' . $item->URL . '">' . $item->Title . '</a>';
				}
				if (isset($item->children)) {
					echo '<ul>';
					$this->menu_print($path,$item->children);
					echo '</ul>';
				}
				echo '</li>';
			}
			if ($parent) {
				echo '</ul>';
			}
		}

		// TODO: Delete a menu item and all its children.
		function menu_delete($id) {
			echo 'Menu deletion is not yet in place.';
		}

		// Create a database node.
		function node_create($data) {
			return $this->parent->mysql->query("INSERT INTO cms_node (Data) VALUES ('" . $this->parent->mysql->escape($data) . "')");
		}

		// Load a node. Prefers file nodes over database nodes, unless it's the home node.
		function node_load($path = 'home') {
			if (strlen($path) == 0) {
				$path = 'home';
				$db_home = $this->node_load_database($path);
				if ($db_home != false) {
					return $db_home;
				}
			}
			$node_file = 'cms/nodes/' . $path . '.php';
			$real = realpath(dirname(__FILE__));
			if (file_exists($real . '/' . $node_file) && realpath(substr(realpath($real . '/' . $node_file),0,strlen($real . '/nodes/'))) == realpath($real . '/nodes/')) {
				return $this->node_load_file($real . '/' . $node_file);
			} else {
				return $this->node_load_database($path);
			}
		}

		function node_load_file($path) {
			if (!file_exists($path)) {
				return false;
			}
			ob_start();
			require_once($path);
			$data = ob_get_contents();
			ob_end_clean();
			return $data;
		}

		function node_load_database($path) {
			$result = $this->parent->mysql->query("SELECT * FROM cms_node WHERE Path='" . $this->parent->mysql->escape($path) . "'");
			if ($this->parent->mysql->num_rows($result) == 0) {
				return false;
			}
			$node = $this->parent->mysql->fetch_object($result);
			return $node->Data;
		}

		// Delete a database node.
		function node_delete($id) {
			return $this->parent->mysql->query("DELETE FROM cms_node WHERE ID='" . $this->parent->mysql->escape($id) . "'");
		}
	}
?>
