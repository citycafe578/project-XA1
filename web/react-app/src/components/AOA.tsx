import speed from "../img/speed.png";
import roll from "../img/AOA1.png";
import depression from "../img/depression.png";
import "./AOA.css";
import { useState } from "react";

function changeOpposite() {
    const [now_site, setNowSite] = useState(0);

    const updateSite = () => {
        if(now_site === 50) {
            setNowSite(-1);
        }else if (now_site === -50) {
            setNowSite(1);
        }else {
            setNowSite((prevNowSite) => prevNowSite + 10);
        }
    };
    setTimeout(updateSite, 100);

    return (
        <div className="AOAsystem data_display" id="AOA_div">
            <div>
            <img src={speed} alt="speed picture" className="AOAsystem" id="speed" style={{ transform: `translate(0px, ${now_site}px)` }}/>
        </div>
        <div id="roll_div">
            <img src={roll} alt="roll picture" className="AOAsystem" id="roll" />
        </div>
        <div>
            <img src={depression} alt="depression picture" className="AOAsystem" id="depression" />
        </div>
        </div>
    );   
}



export default changeOpposite;
