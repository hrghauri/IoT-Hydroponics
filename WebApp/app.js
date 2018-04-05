firebase.auth().onAuthStateChanged(function(user) {
  if (user) {
    // User is signed in.
    document.getElementById("user-div").style.display = "block";
	document.getElementById("login-div").style.display = "none";
	document.getElementById("nav-div").style.display ="block";
	
	var user = firebase.auth().currentUser;
	
	if(user != null){
		var email_id = user.email;
		
		document.getElementById("welcome-current-user").innerHTML = "Welcome User : " + email_id;
	}
	
  } else {
	document.getElementById("user-div").style.display = "none";
	document.getElementById("login-div").style.display = "block";
	document.getElementById("nav-div").style.display ="none";
  }
});




function login(){
	var userEmail = document.getElementById("email_field").value;
	var userPass = document.getElementById("password_field").value;
	
	//window.alert(userEmail + " " + userPass);
	
	firebase.auth().signInWithEmailAndPassword(userEmail, userPass).catch(function(error) {
	  // Handle Errors here.
		var errorCode = error.code;
		var errorMessage = error.message;
		
		window.alert("Error : " + errorMessage);
	  // ...
	});
}


function logout(){
	firebase.auth().signOut().then(function() {
  // Sign-out successful.
	}).catch(function(error) {
		var errorCode = error.code;
		var errorMessage = error.message;
		
		window.alert("Error : " + errorMessage);
	});
}