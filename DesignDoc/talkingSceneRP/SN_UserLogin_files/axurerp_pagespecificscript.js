for(var i = 0; i < 12; i++) { var scriptId = 'u' + i; window[scriptId] = document.getElementById(scriptId); }

$axure.eventManager.pageLoad(
function (e) {

});

u10.style.cursor = 'pointer';
$axure.eventManager.click('u10', function(e) {

if (true) {

	NewTab($axure.globalVariableProvider.getLinkUrl('SN_StorySelect.html'), "");

}
});
gv_vAlignTable['u1'] = 'center';gv_vAlignTable['u3'] = 'center';gv_vAlignTable['u5'] = 'center';gv_vAlignTable['u7'] = 'center';