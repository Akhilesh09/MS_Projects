<html> 
<head>
    <link rel="stylesheet" href="/styles.css">
    <link href="http://db.onlinewebfonts.com/c/f0daf632a7f9d6b0c1741305664fefb4?family=Lint+McCree+Intl+BB" rel="stylesheet" type="text/css"/> 
    <link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/4.5.2/css/bootstrap.min.css">
    <script src="https://ajax.googleapis.com/ajax/libs/jquery/3.5.1/jquery.min.js"></script>
    <script src="https://cdnjs.cloudflare.com/ajax/libs/popper.js/1.16.0/umd/popper.min.js"></script>
    <script src="https://maxcdn.bootstrapcdn.com/bootstrap/4.5.2/js/bootstrap.min.js"></script>
    <script src="http://ajax.googleapis.com/ajax/libs/jquery/1.7.1/jquery.min.js" type="text/javascript"></script>
</head>    
<style>
 /* vertical slider */
 input#myRange3
{
    -webkit-appearance:slider-vertical;
    height: 50%;
    float: right;
/*    position: absolute;*/
/*        z-index: 1;*/
}
    
.polaroid {
/*  width: 80%;*/
  background-color: white;
  margin: 3%;
  box-shadow: 0 8px 16px 0 rgb(0,0,0);
  
}
    
.polaroid:hover{
    background-color: lightblue;
}
    
.polaroid:active{
    transform: translateY(4px);
}
    
.tab {
  overflow: hidden;
  border: 1px solid #ccc;
  background-color: #f1f1f1;
}

/* Style the buttons inside the tab */
.tab button {
  background-color: inherit;
/*  float: left;*/
  border: none;
  outline: none;
  cursor: pointer;
  padding: 14px 16px;
  transition: 0.3s;
  font-size: 17px;
}

/* Change background color of buttons on hover */
.tab button:hover {
  background-color: #ddd;
}

/* Create an active/current tablink class */
.tab button.active {
  background-color: #ccc;
}

/* Style the tab content */
.tabcontent {
  display: none;
  padding: 6px 12px;
  border: 1px solid #ccc;
  border-top: none;
}    

select {
  background-color: skyblue;
  color: white;
  padding: 6px;
  width: 250px;
  border: none;
  font-size: 20px;
  box-shadow: 0 5px 25px rgba(0, 0, 0, 0.2);
  -webkit-appearance: button;
  appearance: button;
  outline: none;
}    

</style>

<body onload="init();"> 
    
     <center>
         
    <h1> Comic Interface</h1> <br>
    <div id="n-box" style="display:none">
        
        
            
         <table>
             
            <tr>
                <td> <div id="diag1" class="triangle-right" style="border: 2.5px solid black"  ><textarea style="border: none; overflow:hidden" id="inside1" contenteditable="true">Enter Dialogue</textarea></div>
                <div class="inner-tri1"></div>
                <div class="outer-tri1"></div>
                </td>
                <td> <div id="diag2" class="triangle-right " style="border: 2.5px solid black"  ><textarea style="border: none; overflow:hidden" id="inside2" contenteditable="true">Enter Dialogue</textarea></div>
                <div class="inner-tri2"></div>
                <div class="outer-tri2"></div>
                </td>
            </tr>
             
            <tr>

            </tr>
             
            <tr>
                <td><canvas id="c3"></canvas></td>
                <td><canvas id="c4"></canvas></td>
            </tr>
             
             <tr id="dropdown">
            </tr> 
                
              <input type="range" min="0" max="1" step="0.001" value="1" step="10"  onchange="setup();" id="myRange3"> 
             
        </table> 
        
                
       
        
       
        
        
        <div align="center" class="slidecontainer">
            
            <input type="range" min="0" max="1" step="0.001" value="0.366" class="slider" onchange="setup();" id="myRange">  <br> <br> <br>
            
            
            
        </div>
        

    </div>
         
     <div id="selectedChars" style="display:none;">
        <h2> Selected Characters</h2>
    </div>
         
    <h2> Select 2 Characters</h2>
    <h6> Re-click Character to replace</h6>
         
    <div class="tab">
    
    <?php

    $dir = "assets/CartoonFolder/";
        
    $emotions=[];
        
    $categories = scandir( $dir );
    $categories=array_splice($categories,2);

        
    foreach( $categories as $category ):
        $res= "'".$category."'";
         echo "<button class='tablinks' name='".$category."' onclick='openCity(event,this.name)'>".$category." </button>";
        
        $characters = scandir( $dir.$category );
        $characters=array_splice($characters,2);
    endforeach;

    ?>
        
    </div>
         
    
    <?php
         
    foreach( $categories as $category ):
         echo "<div class='tabcontent' id='".$category."'>";
        $characters = scandir( $dir.$category );
        $characters=array_splice($characters,2);
         
        foreach( $characters as $character ):
        $emos=scandir( $dir.$category."/".$character );
        $emotions[$character]=array_splice($emos,2);
         
        echo "<figure style='display:inline;margin:2%;padding:0;float:left;'>";
        echo "<img src='" . $dir.$category."/".$character."/default.png" . "' class='polaroid' width='250' height='250' onclick='select_img(this.src,false,false);'/>";
        echo "<figcaption> <b>".preg_replace('/[0-9]+/', '', $character)."</b> </figcaption>";
        echo "</figure>";
            
        
    endforeach;
         echo "</div>";
         
    endforeach;
         
    ?>

   
    <br><br>

    </center>

    <script src="node_modules/pixi.js/dist/pixi.min.js"></script>
    
    <script src="dist/PIXI.TextInput.js" type="text/javascript"></script>
    
    <script src="dist/PIXI.TextInput.min.js" type="text/javascript"></script>
    
    <script>
        
        function init()
        {
            document.getElementsByClassName("tablinks")[0].click();
        }
        
        function openCity(evt, cityName) {
          var i, tabcontent, tablinks;
          tabcontent = document.getElementsByClassName("tabcontent");
        for (i = 0; i < tabcontent.length; i++) {
            tabcontent[i].style.display = "none";
          }
          tablinks = document.getElementsByClassName("tablinks");
          for (i = 0; i < tablinks.length; i++) {
            tablinks[i].className = tablinks[i].className.replace(" active", "");
          }
        for (i = 0; i < tablinks.length; i++) {
            tablinks[i].className = tablinks[i].className.replace(" active", "");
          }
          document.getElementById(cityName).style.display = "block";
          evt.currentTarget.className += " active";
        }
    </script>
    
    <script>
        
    
    let Application = PIXI.Application,
    loader = PIXI.loader,
    resources = PIXI.loader.resources,
    Sprite = PIXI.Sprite;
    var images= new Array();
    var prev_del=0;
        
        
    function select_img(val,truth_val,emo_val)
        {
            var selectedDiv=document.getElementById("selectedChars");   
            selectedDiv.style.display="inline";
            var img_ele;
            
            var emotions_list= <?php echo json_encode($emotions); ?>;
            
            var flag=0;
            for(var i=0;i<images.length;i++)
            {
                basePath=val.split("/")[6];
                
                if(images[i].includes(basePath))
                    {
                    prev_del=i;
                    flag=1;
                    }
            }
            
            var old_selection=document.getElementById("selected"+prev_del);
            
            if(!flag && old_selection==null)
                {
                    
                img_ele=document.createElement("img");
                img_ele.setAttribute("id","selected"+prev_del);
                img_ele.setAttribute("src",val);
                img_ele.style.display="inline";
                img_ele.setAttribute("width",250);
                img_ele.setAttribute("height",250);
                }
            

            if(images.length<2)
            {
                if(!flag)
                {
                    images.splice(prev_del++,0,val);
                    try
                    {
                        
                    selectedDiv.appendChild(img_ele);
                        
                    }
                    catch
                    {
                        
                     old_selection.setAttribute("src",val);   
                     old_selection.style.display="inline";
                    
                    }
                }
                else
                {
                    document.getElementById("n-box").style.display="none";
                    images.splice(prev_del,1);
                    selectedDiv.removeChild(old_selection);
                }
                
                
            }
            else if(images.length==2 )
                {
                   
                    if(flag)
                    {

                        document.getElementById("n-box").style.display="none";
                        images.splice(prev_del,1);

                        old_selection.style.display="none";

                    }
                        
                }
            
            
            
            if(images.length==2)
                {
                    if(truth_val)
                    {
                        current_img=images[parseInt(val[val.length-1])];

                        new_img=current_img.split("/");
                        new_img.splice(new_img.length-1,1,emo_val+".png");
                        new_img=new_img.join("/");

                        images.splice(val[val.length-1],1,new_img);

                    }
                    
                     
                    for(var i=0;i<images.length;i++)
                        {
                            try
                            {
                                loader.add(images[i]);
                                basePath=images[i].split("/")[6];
                                current_emo=images[i].split("/")[7].split(".")[0];

                                try
                                {
                                    var old_child=document.getElementById("emo"+i);
                                }
                                catch{}



                                var parent=document.getElementById("dropdown");   
                                var td_ele=document.createElement("td");
                                td_ele.setAttribute("id","emo"+i);
                                var select=document.createElement("select");
                                select.setAttribute("id","emolist"+i);
                                select.setAttribute("onchange","select_img(this.id,true,this.value)");
                                td_ele.appendChild(select);

                                try
                                {
                                    parent.replaceChild(td_ele,old_child);

                                }
                                catch
                                {
                                    parent.appendChild(td_ele);
                                }

                                var option_ele=document.createElement("option");
                                option_ele.innerHTML="Choose emotion";
                                select.appendChild(option_ele);
                                if(current_emo=="default")
                                    option_ele.setAttribute("selected","selected");
                            
                            

                                for(var j=0;i<emotions_list[basePath].length;j++)
                                {
                                    var option_ele=document.createElement("option");
                                    var value=emotions_list[basePath][j].split(".")[0];
                                    if(value==current_emo && current_emo!="default")
                                        option_ele.setAttribute("selected","selected");
                                    option_ele.innerHTML=value;
                                    select.appendChild(option_ele);
                                }
                            }
                            catch
                            {
                                continue;
                            }
                        }
                     loader.load(setup);  
                     document.getElementById("n-box").style.display="block";
            }
            
        }
        
        
    //This `setup` function will run when the image has loaded
    function setup() {
    
      // total width
      const w =500;

     //Create the obama sprite
      let image0 = new Sprite(resources[images[0]].texture);

      let image1= new Sprite(resources[images[1]].texture);

      let canvas_height=w/((image0.width/image0.height)+(image1.width/image1.height));

      // image0 initial width
      const w0=image0.width*canvas_height/image0.height;
        
      //image1 initial width
      const w1=image1.width*canvas_height/image1.height;
        
      const total_width=w0+w1;
        
      const total_height=canvas_height+100;
        
        

        // image0 canvas
        let app1 = new Application({ 
            width: w0, 
            height:canvas_height,                       
            view : document.getElementById("c3"),
            backgroundColor: 0xFFFFFF
          }
        );
        
        // image1 canvas
        let app2 = new Application({ 
            width: w1, 
            height:canvas_height,                       
            view : document.getElementById("c4"),
            backgroundColor: 0xFFFFFF
          }
        );
        
        // initial horizontal slider value
        $("#myRange").css('width',w);

        // current horizontal slider value 
        var t=document.getElementById("myRange").value;
        
        // current vertical slider value 
        var vertical_amount=1-(document.getElementById("myRange3").value);
        
        // image1 becomes bigger
        if(t <= (w0/w))
        {

        var s0=t*w/w0;
        var s1=1+(2*(1-s0)*w0/w1);

        image0.scale.set(s0*canvas_height/image0.height);
        image1.scale.set(s1*canvas_height/image1.height);

        // stop vertical translation when same height
        if(image0.height>canvas_height*(1-2*vertical_amount))
           {
            vertical_amount=1-(image0.height/canvas_height);
           }


        image0.x = 0;
        image0.y = (1-s0)*canvas_height -vertical_amount*canvas_height;

        image1.x = 0 ;
        image1.y = (1-s1)*canvas_height/image1.height; 

        // resize canvases
        app1.renderer.resize(image0.width, canvas_height*(1-vertical_amount));
        app2.renderer.resize(total_width-image0.width, canvas_height*(1-vertical_amount));

        // resize speech bubbles
        $("#diag1").css('width',image0.width*0.99);
        $("#diag2").css('width',(total_width-image0.width)*0.99);

        $("#inside1").css('width',image0.width*0.8);
        $("#inside2").css('width',(total_width-image0.width)*0.8);
            
        $("#emolist0").css('width',image0.width);
        $("#emolist1").css('width',(total_width-image0.width));


        }
        // image0 becomes bigger
        else
        {

        var s1 =   (1-t)*w/w1;
        var s0 =  1+(2*(1-s1)*w1/w0);

        image0.scale.set(s0*canvas_height/image0.height);
        image1.scale.set(s1*canvas_height/image1.height);

        // stop vertical translation when same height
        if(image1.height>canvas_height*(1-2*vertical_amount))
           {
            vertical_amount=1-(image1.height/canvas_height);
           }

        image0.x = 0;
        image0.y = (1-s0)*canvas_height/image0.height; 

        image1.x = 0;
        image1.y = (1-s1)*canvas_height-vertical_amount*canvas_height;

        // resize canvases
        app1.renderer.resize(total_width-image1.width, canvas_height*(1-vertical_amount));
        app2.renderer.resize(image1.width, canvas_height*(1-vertical_amount));

        // resize speech bubbles
        $("#diag1").css('width',(total_width-image1.width)*0.99);
        $("#diag2").css('width',image1.width*0.99);

        $("#inside2").css('width',image1.width*0.8);
        $("#inside1").css('width',(total_width-image1.width)*0.8);
            
        $("#emolist0").css('width',(total_width-image1.width));
        $("#emolist1").css('width',image1.width);
            

        }
        
        // reduce fontsize as space increases
        $("#inside1").css('font-size',16*(1-vertical_amount+0.5)+'px')
        $("#inside2").css('font-size',16*(1-vertical_amount+0.5)+'px')

        // increase height of speech bubbles
        $("#diag1").css('height',(total_height-canvas_height*(1-vertical_amount)));
        $("#diag2").css('height',(total_height-canvas_height*(1-vertical_amount)));

        $("#inside1").css('height',(total_height-canvas_height*(1-vertical_amount))*0.8);
        $("#inside2").css('height',(total_height-canvas_height*(1-vertical_amount))*0.8);
        
                
        var rect = document.getElementById("diag1").getBoundingClientRect();
        $(".inner-tri1").css('left',(rect.right-rect.left)/2+"px");
        $(".outer-tri1").css('left',-5+(rect.right-rect.left)/2+"px");
        
        var rect = document.getElementById("diag2").getBoundingClientRect();
        $(".inner-tri2").css('left',(rect.right-rect.left)/2+"px");
        $(".outer-tri2").css('left',-5+(rect.right-rect.left)/2+"px");
        

        //Add the obama to the stage
        app2.stage.addChild(image1);
        app1.stage.addChild(image0);

    }
        
    
        window.addEventListener("resize", function() {
            app2.renderer.resize(window.innerWidth, window.innerHeight);
            app1.renderer.resize(window.innerWidth, window.innerHeight);
        }); 

    
            
</script> 
</body>
</html> 