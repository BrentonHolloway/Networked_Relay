$(document).ready(function(){
	$(".relay").click(function(e){
		var data = $(this).serialize();
		if(this.checked != true){
			data = this.name + "=off";
		}
		$.post("relay", data);
	});
});