<html>
<head>
<title>すべての灯籠一覧</title>
<script type="text/javascript" src="http://maps.google.com/maps/api/js?key=AIzaSyBf_Oxw8v8izVcB45rg7lORSm7QgmScJbA&sensor=true" charset="utf-8"></script>

<?php
$mysqli=mysqli_connect('localhost','root','Keyu3102','test2');    //MySQLのtest2へのアクセス
if (mysqli_connect_errno()) {
	printf("Connect failed: %s\n", mysqli_connect_error());
	exit();
}
	$sql = "SELECT * FROM ne";                                        //テーブルneを選択
	$Result=mysqli_query($mysqli,$sql);                               //結果の代入
	$count=0;
  while($row = mysqli_fetch_assoc($Result)){                    //テーブルないすべてのデータを出力するまでループ
		if ($count==0){
			$placepoint="{ lat:\"$row[NPoint]\", lng: \"$row[EPoint]\", title:\"marker-1\" }";
			$count++;
		} else {
				$placepoint="{ lat:\"$row[NPoint]\", lng: \"$row[EPoint]\", title:\"marker-1\" }\n,$placepoint";
		}
	}
 ?>

<script type="text/javascript">
function initialize() {
  // Google Mapで利用する初期設定用の変数
  var latlng = new google.maps.LatLng(34.0527, 134.563);
	var mapOptions = {
		zoom: 12,
		mapTypeId: google.maps.MapTypeId.ROADMAP,
		center: latlng
	};
	// GoogleMapの生成
	var gmap = new google.maps.Map(document.getElementById("map"), mapOptions);

	// マーカー座標
	var markerData = [
		<?php echo $placepoint; ?>
	];
	for (i = 0;i < markerData.length;i++) {
		// マーカーを生成
		var marker = new google.maps.Marker({
		    position: new google.maps.LatLng(markerData[i].lat, markerData[i].lng),
		    title: markerData[i].title
		});
		// マーカーを地図に表示
		marker.setMap(gmap);
	}
}
</script>
</head>
<body onload="initialize()">																																													<!--ロード完了時にGoogleMapのJSが実行される-->
	<div id="map" style="width: 630px; height: 480px;"></div>
</body>
</html>
