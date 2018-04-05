var pageCount = 30;

var query = firebase.database().ref("Readings").orderByChild('Time').limitToLast(pageCount);

query.on('value', function(snap) {
        var items = snap.val();
        var keys = Object.keys(items);
        if(keys != null) {
            for(var i = keys.length - 1; i >= 0; i--) {
                var key = keys[i];
                var values = items[key];

                var PH = values['PH'];
                var Time = values['Time'];
                var image = values['Image'];
                var Image_Title = image['Image_Title'];
                var Image_URL = image['Image_URL'];
				
                // Append rows on the table.
                //var row = '<tr><td>' + Time + '</td><td>' + PH + '</td><td><a href='+ Image_URL + '>' + Image_URL + '</a></td><td>' + Image_Title + '</td></tr>';
                //document.getElementById('table-content').appendChild(row);
				//console.log(row);
				
				var row = document.createElement("tr");
				
				
				var td = document.createElement("td");
				var txt = document.createTextNode(Time);
				td.appendChild(txt);
				row.appendChild(td);
				
				
				td = document.createElement("td");
				txt = document.createTextNode(PH);
				td.appendChild(txt);
				row.appendChild(td);
				
				
				td = document.createElement("td");
				var link = document.createElement("a");
				link.setAttribute("href", Image_URL);
				link.setAttribute("target","_blank");
				txt = document.createTextNode(Image_URL);
				link.appendChild(txt);
				td.appendChild(link);
				row.appendChild(td);
				
				td = document.createElement("td");
				txt = document.createTextNode(Image_Title);
				td.appendChild(txt);
				row.appendChild(td);
				
				document.getElementById('table-content').appendChild(row);		
                // Check Last object.
                if(i == 0) {
                    lastTime = Time;
                }
            }
        }
    });
