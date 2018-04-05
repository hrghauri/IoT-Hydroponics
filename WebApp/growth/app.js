	var dataPoints1 = [];
	
	const dbRefObject = firebase.database().ref("Growth").limitToLast(8);
	
	dbRefObject.on('child_added', function(snapshot){
		//console.log(snapshot.val());
		
		//console.log(snapshot.child('Growth_Area').val());
		
		
		var datapoint;
		var firebaseRecord = snapshot.val();
		
		if (firebaseRecord['DeseaseFound'] == 'Yes'){
			datapoint = {x: new Date(snapshot.child('Date').val()), y:  snapshot.child('Growth_Area').val(), indexLabel: "Desease Detected",markerColor: "DarkSlateGrey", markerType: "cross" }
		}
		else{
			datapoint = {x: new Date(snapshot.child('Date').val()), y:  snapshot.child('Growth_Area').val()}
		}
		
		
		dataPoints1.push(datapoint);
		
		var chart = new CanvasJS.Chart("chartContainer", {
			animationEnabled: true,
			theme: "light2",
			title:{
				text: "Plant Growth"
			},
			axisY:{
				includeZero: false
			},
			data: [{        
				type: "line",       
				dataPoints: dataPoints1
			}]
		});
		chart.render();
		
	});
	
	
	

			
	
	

