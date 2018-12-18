$('td.DHCP input').click(function(){
	if($(this).attr('id') == 'dynamic'){
		$(".num input").prop("disabled", true);
	}
	if($(this).attr('id') == 'static'){
		$(".num input").prop("disabled", false);
	}
});

$('td.macSet input').click(function(){
	if($(this).attr('id') == 'defmac') {
		$("#macaddr").prop("disabled", true);
	}
	if($(this).attr('id') == 'custommac') {
		$("#macaddr").prop("disabled", false);
	}
});

$(document).ready(function(){
	$("form").submit(function(e){
		alert("Submitted");
		e.preventDefault();
		var data = $("#networkForm :input").serializeArray();
		console.log(data);
	});
});