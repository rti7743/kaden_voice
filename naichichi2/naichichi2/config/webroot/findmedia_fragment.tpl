<?lua
	local from = out["media"]["header_from"];
	local to   = out["media"]["header_to"];

	for(i=from,to){ 
	local key=""..i.."";
	if( not out["media"][key]){ break; }
	local value = out["media"][key];
?>
	<div class="img" id="img_<?= key ?>">
		<div><a href="javascript:void(0)" onclick="mediaplay(<?= key ?>)">
			<img width="255" height="255"  src='data:image/jpeg;base64,<?= value["image"] ?>'><br />
			<div class="msg" ><span class="select_mark"><?= key ?></span><?= value["title"] ?></div>
		</a></div>
	</div>
	<div style="display: none;position: absolute2;" id="popup_<?= key ?>">
		<table>
			<tr>
				<th>ディレクトリ<th><td><?= value["dir"] ?></td>
			</tr>
			<tr>
				<th>ファイル名<th><td><?= value["filename"] ?></td>
			</tr>
			<tr>
				<th>アーティスト<th><td><?= value["artist"] ?></td>
			</tr>
			<tr>
				<th>アルバム<th><td><?= value["album"] ?></td>
			</tr>
			<tr>
				<th>エイリアス<th><td><?= value["alias"] ?></td>
			</tr>
		</table>
	</div>
<?lua } ?>
