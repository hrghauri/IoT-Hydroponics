function lightsOnFunction(){
	firebase.database().ref('UserInput/Light').set("On");
}

function lightsOffFunction(){
	firebase.database().ref('UserInput/Light').set("Off");
}

