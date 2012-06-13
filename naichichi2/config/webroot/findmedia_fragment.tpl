<?lua
	local from = out["media"]["header_from"];
	local to   = out["media"]["header_to"];

	for(i=from,to){ 
	local key=""..i.."";
	if( not out["media"][key]){ break; }
	local value = out["media"][key];
?>
	<div class="item" id="item_<?= key ?>">
		<div><a href="javascript:void(0)">
			<img width="255" height="255"  src='data:image/jpeg;base64,<?= value["image"] ?>'><br />
			<div class="msg" ><span class="select_mark"><?= key ?></span><?= value["title"] ?></div>
		</a></div>
		<div class="iteminfo"  id="iteminfo_<?= key ?>" style="display: none;">
			<span class="iteminfo_filename"><?= value["filename"] ?></span>
			<table>
				<tr><th>Folder</th><td><span class="iteminfo_dir"><?= value["dir"] ?></span></td></tr>
				<tr><th>Artist</th><td><span class="iteminfo_artist"><?= value["artist"] ?></span></td></tr>
				<tr><th>Album</th><td><span class="iteminfo_album"><?= value["album"] ?></span></td></tr>
				<tr><th>Alias</th><td><span class="iteminfo_alias"><?= value["alias"] ?></span></td></tr>
				<tr><th>Rank</th><td><span class="iteminfo_rank"><?= value["rank"] ?></span></td></tr>
			</table>
		</div>
	</div>
<?lua } ?>
