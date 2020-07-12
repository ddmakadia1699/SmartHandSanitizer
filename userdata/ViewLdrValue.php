<!DOCTYPE html>
<html>
	<head>
		<title>User Data</title>
		<meta name="viewport" content="width=device-width, initial-scale=1.0">
		<meta charset="utf-8">
		<link href="css/bootstrap.min.css" rel="stylesheet">
		<script src="js/bootstrap.min.js"></script>
		<!-- Script for updating pages without refreshing the page -->
		<script src="jquery.min.js"></script>
		<script>
			$(document).ready(function() {
				setInterval(function(){get_data()},5000);
				function get_data()
				{
					jQuery.ajax({
						type:"GET",
						url: "read_db.php",
						data:"",
						beforeSend: function() {
						},
						complete: function() {
						},
						success:function(data) {
							$("table").html(data);
						}
					});
				}
			});
		</script>
		<style>
			
			html {
				font-family: Arial;
				display: inline-block;
				margin: 0px auto;
				text-align: center;
			}

			ul.topnav {
				list-style-type: none;
				margin: auto;
				padding: 0;
				overflow: hidden;
				background-color: #4CAF50;
				width: 70%;
			}

			ul.topnav li {float: left;}

			ul.topnav li a {
				display: block;
				color: white;
				text-align: center;
				padding: 14px 16px;
				text-decoration: none;
			}

			ul.topnav li a:hover:not(.active) {background-color: #3e8e41;}

			ul.topnav li a.active {background-color: #333;}

			ul.topnav li.right {float: right;}

			@media screen and (max-width: 600px) {
				ul.topnav li.right, 
				ul.topnav li {float: none;}
			}
			
			img {
				display: block;
				margin-left: auto;
				margin-right: auto;
			}
			table {
				border-collapse: collapse;
				width: 100%;
				color: #1f5380;
				font-family: monospace;
				font-size: 20px;
				text-align: left;
			} 
			th {
				background-color: #1f5380;
				color: white;
			}
			tr:nth-child(even) {background-color: #f2f2f2}
		</style>
	</head>
	<body>
		<h2>Smart Hand Sanitizer</h2>
		<ul class="topnav">
			<li><a href="/esp32/home.php">Home</a></li>
			<li><a href="/esp32/user data.php">User Data</a></li>
			<li><a href="/esp32/registration.php">Registration</a></li>
			<li><a href="/esp32/read tag.php">Read Tag ID</a></li>
			<li><a class="active" href="userdata/ViewLdrValue.php">User Temp</a></li>
		</ul>
		<h3 align="center" style="color:#1f5380;">User Temperature</h3>
		<table>
			<tr>
				<th>No</th> 
				<th>ID</th>
				<th>Temp Value</th> 
				<th>Date</th>
				<th>Time</th>
			</tr>
		</table>
	</body>
</html>