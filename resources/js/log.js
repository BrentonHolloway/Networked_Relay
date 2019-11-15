$(document).ready(function(){
	$("form").submit(function(e){
		alert("Submitted");
		e.preventDefault();
	});
});