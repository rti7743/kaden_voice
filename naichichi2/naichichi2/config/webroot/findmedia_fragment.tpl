<?lua for(i=1,99){ 
	local key=""..i.."";
	if( not out["records"][key]){ break; }
	local value = out["records"][key];
?>
	<div class="img" id="img-1">
		<div><a href="javascript:void(0)" onclick="mediaplay(<?= key ?>)">
			<img width="255" height="255"  src='data:image/jpeg;base64,<?= value["image"] ?>'><br />
			<div class="msg" ><span class="select_mark"><?= key ?></span><?= value["title"] ?></div>
		</a></div>
	</div>
<?lua } ?>
