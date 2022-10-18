


xcopy	/y/s	.\Engine\Public\*.h			.\Reference\Headers\
xcopy	/y	.\Engine\Bin\*.lib				.\Reference\Librarys\
xcopy	/y	.\Engine\ThirdPartyLib\*.lib	.\Reference\Librarys\
xcopy	/y	.\Engine\Bin\*.dll				.\Client\Bin\

xcopy	/y/s	.\Engine\Public\*.h			..\ImGui\Reference\Headers\
xcopy	/y	.\Engine\Bin\*.lib				..\ImGui\\Reference\Librarys\
xcopy	/y	.\Engine\ThirdPartyLib\*.lib	..\ImGui\Reference\Librarys\
xcopy	/y	.\Engine\Bin\*.dll				..\ImGui\Client\Bin\