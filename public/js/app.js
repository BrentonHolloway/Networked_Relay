
$(document).ready(function(){$("form").submit(function(t){alert("Submitted"),t.preventDefault()})});
$("td.DHCP input").click(function(){"dynamic"==$(this).attr("id")&&$(".num input").prop("disabled",!0),"static"==$(this).attr("id")&&$(".num input").prop("disabled",!1)}),$("td.macSet input").click(function(){"defmac"==$(this).attr("id")&&$("#macaddr").prop("disabled",!0),"custommac"==$(this).attr("id")&&$("#macaddr").prop("disabled",!1)}),$(document).ready(function(){$("#networkForm").submit(function(t){t.preventDefault();var i=$("#networkForm :input").serialize();$.post("network",i)})});
$(document).ready(function(){$(".relay").click(function(e){var i=$(this).serialize();1!=this.checked&&(i=this.name+"=off"),$.post("relay",i)})});
$("#rt").click(function(){$("#rsd").attr("disabled",!$("#rsd").attr("disabled")),$("#red").attr("disabled",!$("#red").attr("disabled")),$("#rst").attr("disabled",!$("#rst").attr("disabled")),$("#ret").attr("disabled",!$("#ret").attr("disabled"))});