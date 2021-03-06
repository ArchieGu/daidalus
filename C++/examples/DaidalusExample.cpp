/*
 * Copyright (c) 2019-2020 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
/**

Notices:

Copyright 2016 United States Government as represented by the
Administrator of the National Aeronautics and Space Administration. No
copyright is claimed in the United States under Title 17,
U.S. Code. All Other Rights Reserved.

Disclaimers

No Warranty: THE SUBJECT SOFTWARE IS PROVIDED "AS IS" WITHOUT ANY
WARRANTY OF ANY KIND, EITHER EXPRESSED, IMPLIED, OR STATUTORY,
INCLUDING, BUT NOT LIMITED TO, ANY WARRANTY THAT THE SUBJECT SOFTWARE
WILL CONFORM TO SPECIFICATIONS, ANY IMPLIED WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, OR FREEDOM FROM
INFRINGEMENT, ANY WARRANTY THAT THE SUBJECT SOFTWARE WILL BE ERROR
FREE, OR ANY WARRANTY THAT DOCUMENTATION, IF PROVIDED, WILL CONFORM TO
THE SUBJECT SOFTWARE. THIS AGREEMENT DOES NOT, IN ANY MANNER,
CONSTITUTE AN ENDORSEMENT BY GOVERNMENT AGENCY OR ANY PRIOR RECIPIENT
OF ANY RESULTS, RESULTING DESIGNS, HARDWARE, SOFTWARE PRODUCTS OR ANY
OTHER APPLICATIONS RESULTING FROM USE OF THE SUBJECT SOFTWARE.
FURTHER, GOVERNMENT AGENCY DISCLAIMS ALL WARRANTIES AND LIABILITIES
REGARDING THIRD-PARTY SOFTWARE, IF PRESENT IN THE ORIGINAL SOFTWARE,
AND DISTRIBUTES IT "AS IS."

Waiver and Indemnity: RECIPIENT AGREES TO WAIVE ANY AND ALL CLAIMS
AGAINST THE UNITED STATES GOVERNMENT, ITS CONTRACTORS AND
SUBCONTRACTORS, AS WELL AS ANY PRIOR RECIPIENT.  IF RECIPIENT'S USE OF
THE SUBJECT SOFTWARE RESULTS IN ANY LIABILITIES, DEMANDS, DAMAGES,
EXPENSES OR LOSSES ARISING FROM SUCH USE, INCLUDING ANY DAMAGES FROM
PRODUCTS BASED ON, OR RESULTING FROM, RECIPIENT'S USE OF THE SUBJECT
SOFTWARE, RECIPIENT SHALL INDEMNIFY AND HOLD HARMLESS THE UNITED
STATES GOVERNMENT, ITS CONTRACTORS AND SUBCONTRACTORS, AS WELL AS ANY
PRIOR RECIPIENT, TO THE EXTENT PERMITTED BY LAW.  RECIPIENT'S SOLE
REMEDY FOR ANY SUCH MATTER SHALL BE THE IMMEDIATE, UNILATERAL
TERMINATION OF THIS AGREEMENT.
 **/

#include "Daidalus.h"
#include <fstream>
#include <string>
using namespace larcfm;
using namespace std;

void printDetection(Daidalus& daa) {
  // Aircraft at index 0 is ownship
  for (int ac_idx=1; ac_idx <= daa.lastTrafficIndex(); ++ac_idx) {
    TrafficState intruder = daa.getAircraftStateAt(ac_idx);
    for (int alert_level=1;alert_level <= daa.mostSevereAlertLevel(ac_idx);++alert_level) {
      ConflictData det = daa.violationOfAlertThresholds(ac_idx, alert_level);
      if (det.conflict()) {
        std::cout << "Predicted Time to Violation of Alert Thresholds at Level " <<
            alert_level << " with " << intruder.getId() <<
            ": " << Fm2(det.getTimeIn()) << " [s]" << std::endl;
      }
    }
    double t2los = daa.timeToCorrectiveVolume(ac_idx);
    if (ISFINITE(t2los)) {
      std::cout << "Predicted Time to Violation of Corrective Volume with "+intruder.getId()+
          ": "+Fm2(t2los)+" [s]" << std::endl;
    }
  }
}

void printAlerts(Daidalus& daa) {
  // Aircraft at index 0 is ownship
  for (int ac_idx=1; ac_idx <= daa.lastTrafficIndex(); ++ac_idx) {
    int alert = daa.alertLevel(ac_idx);
    if (alert > 0) {
      std::cout << "Alert Level " << alert << " with " <<
          daa.getAircraftStateAt(ac_idx).getId() << std::endl;
    }
  }
}

// Converts numbers, possible NaN or infinities, to string
static std::string num2str(double res, const std::string& u) {
  if (!ISFINITE(res)) {
    return "N/A";
  } else {
    return Fm2(res)+" ["+u+"]";
  }
}

void printBands(Daidalus& daa) {
  TrafficState own = daa.getOwnshipState();
  bool nowind = daa.getWindVelocityTo().isZero();
  std::string hdstr = nowind ? "Track" : "Heading";
  std::string hsstr = nowind ? "Ground Speed" : "Airspeed";
  std::cout << std::endl;

  std::vector<std::string> acs;
  for (int regidx=1; regidx <= BandsRegion::NUMBER_OF_CONFLICT_BANDS; ++regidx) {
    BandsRegion::Region region = BandsRegion::regionFromOrder(regidx);
    daa.conflictBandsAircraft(acs,region);
    std::cout << "Conflict Aircraft for Bands Region " << BandsRegion::to_string(region)
    << ": " << TrafficState::listToString(acs) << std::endl;
  }
  std::cout << std::endl;

  // Horizontal Direction
  double hd_deg = own.horizontalDirection("deg");
  std::cout << "Ownship " << hdstr << ": " << Fm2(hd_deg) << " [deg]"  << std::endl;
  std::cout << "Region of Current " << hdstr << ": " <<
      BandsRegion::to_string(daa.regionOfHorizontalDirection(hd_deg,"deg"))  << std::endl;
  std::cout << hdstr << " Bands [deg,deg]"  << std::endl;
  for (int i=0; i < daa.horizontalDirectionBandsLength(); ++i) {
    Interval ii = daa.horizontalDirectionIntervalAt(i,"deg");
    std::cout << "  " << BandsRegion::to_string(daa.horizontalDirectionRegionAt(i)) << ":\t" << ii.toString(2) << std::endl;
  }
  for (int regidx=1; regidx <= BandsRegion::NUMBER_OF_CONFLICT_BANDS; ++regidx) {
    BandsRegion::Region region = BandsRegion::regionFromOrder(regidx);
    daa.peripheralHorizontalDirectionBandsAircraft(acs,region);
    std::cout << "Peripheral Aircraft for " << hdstr << " Bands Region " << BandsRegion::to_string(region) << ": " <<
        TrafficState::listToString(acs)  << std::endl;
  }
  std::cout << hdstr << " Resolution (right): " << num2str(daa.horizontalDirectionResolution(true,"deg"),"deg")  << std::endl;
  std::cout << hdstr << " Resolution (left): " << num2str(daa.horizontalDirectionResolution(false,"deg"),"deg")  << std::endl;
  std::cout << "Preferred " << hdstr << " Direction: ";
  if (daa.preferredHorizontalDirectionRightOrLeft()) {
    std::cout << "right" << std::endl;
  } else {
    std::cout << "left" << std::endl;
  }
  std::cout << "Recovery Information for Horizontal Speed Bands:"  << std::endl;
  RecoveryInformation recovery = daa.horizontalDirectionRecoveryInformation();
  std::cout << "  Time to Recovery: " <<
      Units::str("s",recovery.timeToRecovery())  << std::endl;
  std::cout << "  Recovery Horizontal Distance: " <<
      Units::str("nmi",recovery.recoveryHorizontalDistance())  << std::endl;
  std::cout << "  Recovery Vertical Distance: " <<
      Units::str("ft",recovery.recoveryVerticalDistance())  << std::endl;

  // Horizontal Speed
  double hs_knot = own.horizontalSpeed("knot");
  std::cout << "Ownship " << hsstr << ": " << Fm2(hs_knot) << " [knot]"  << std::endl;
  std::cout << "Region of Current " << hsstr << ": " <<
      BandsRegion::to_string(daa.regionOfHorizontalSpeed(hs_knot,"knot"))  << std::endl;
  std::cout << hsstr << " Bands [knot,knot]:"  << std::endl;
  for (int i=0; i < daa.horizontalSpeedBandsLength(); ++i) {
    Interval ii = daa.horizontalSpeedIntervalAt(i,"knot");
    std::cout << "  " << BandsRegion::to_string(daa.horizontalSpeedRegionAt(i)) << ":\t" << ii.toString(2) << std::endl;
  }
  for (int regidx=1; regidx <= BandsRegion::NUMBER_OF_CONFLICT_BANDS; ++regidx) {
    BandsRegion::Region region = BandsRegion::regionFromOrder(regidx);
    daa.peripheralHorizontalSpeedBandsAircraft(acs,region);
    std::cout << "Peripheral Aircraft for " << hsstr << " Bands Region " << BandsRegion::to_string(region) << ": " <<
        TrafficState::listToString(acs)  << std::endl;
  }
  std::cout << hsstr << " Resolution (up): " << num2str(daa.horizontalSpeedResolution(true,"knot"),"knot")  << std::endl;
  std::cout << hsstr << " Resolution (down): " << num2str(daa.horizontalSpeedResolution(false,"knot"),"knot")  << std::endl;
  std::cout << "Preferred " << hsstr << " Direction: ";
  if (daa.preferredHorizontalSpeedUpOrDown()) {
    std::cout << "up"  << std::endl;
  } else {
    std::cout << "down"  << std::endl;
  }
  std::cout << "Recovery Information for Horizontal Speed Bands:"  << std::endl;
  recovery = daa.horizontalSpeedRecoveryInformation();
  std::cout << "  Time to Recovery: " <<
      Units::str("s",recovery.timeToRecovery())  << std::endl;
  std::cout << "  Recovery Horizontal Distance: " <<
      Units::str("nmi",recovery.recoveryHorizontalDistance())  << std::endl;
  std::cout << "  Recovery Vertical Distance: " <<
      Units::str("ft",recovery.recoveryVerticalDistance())  << std::endl;

  // Vertical Speed
  double vs_fpm = own.verticalSpeed("fpm");
  std::cout << "Ownship Vertical Speed: " << Fm2(vs_fpm) << " [fpm]"  << std::endl;
  std::cout << "Region of Current Vertical Speed: " <<
      BandsRegion::to_string(daa.regionOfVerticalSpeed(vs_fpm,"fpm"))  << std::endl;
  std::cout << "Vertical Speed Bands [fpm,fpm]:"  << std::endl;
  for (int i=0; i < daa.verticalSpeedBandsLength(); ++i) {
    Interval ii = daa.verticalSpeedIntervalAt(i,"fpm");
    std::cout << "  " << BandsRegion::to_string(daa.verticalSpeedRegionAt(i)) << ":\t" << ii.toString(2) << std::endl;
  }
  for (int regidx=1; regidx <= BandsRegion::NUMBER_OF_CONFLICT_BANDS; ++regidx) {
    BandsRegion::Region region = BandsRegion::regionFromOrder(regidx);
    daa.peripheralVerticalSpeedBandsAircraft(acs,region);
    std::cout << "Peripheral Aircraft for Vertical Speed Bands Region " << BandsRegion::to_string(region) << ": " <<
        TrafficState::listToString(acs)  << std::endl;
  }
  std::cout << "Vertical Speed Resolution (up): " << num2str(daa.verticalSpeedResolution(true,"fpm"),"fpm")  << std::endl;
  std::cout << "Vertical Speed Resolution (down): " << num2str(daa.verticalSpeedResolution(false,"fpm"),"fpm")  << std::endl;
  std::cout << "Preferred Vertical Speed Direction: ";
  if (daa.preferredVerticalSpeedUpOrDown()) {
    std::cout << "up"  << std::endl;
  } else {
    std::cout << "down"  << std::endl;
  }
  std::cout << "Recovery Information for Vertical Speed Bands:"  << std::endl;
  recovery = daa.verticalSpeedRecoveryInformation();
  std::cout << "  Time to Recovery: " <<
      Units::str("s",recovery.timeToRecovery())  << std::endl;
  std::cout << "  Recovery Horizontal Distance: " <<
      Units::str("nmi",recovery.recoveryHorizontalDistance())  << std::endl;
  std::cout << "  Recovery Vertical Distance: " <<
      Units::str("ft",recovery.recoveryVerticalDistance())  << std::endl;

  // Altitude
  double alt_ft =  own.altitude("ft");
  std::cout << "Ownship Altitude: " << Fm2(alt_ft) << " [ft]"  << std::endl;
  std::cout << "Region of Current Altitude: " << BandsRegion::to_string(daa.regionOfAltitude(alt_ft,"ft"))  << std::endl;
  std::cout << "Altitude Bands [ft,ft]:"  << std::endl;
  for (int i=0; i < daa.altitudeBandsLength(); ++i) {
    Interval ii = daa.altitudeIntervalAt(i,"ft");
    std::cout << "  " << BandsRegion::to_string(daa.altitudeRegionAt(i)) << ":\t" << ii.toString(2) << std::endl;
  }
  for (int regidx=1; regidx <= BandsRegion::NUMBER_OF_CONFLICT_BANDS; ++regidx) {
    BandsRegion::Region region = BandsRegion::regionFromOrder(regidx);
    daa.peripheralAltitudeBandsAircraft(acs,region);
    std::cout << "Peripheral Aircraft for Altitude Bands Region " << BandsRegion::to_string(region) << ": " <<
        TrafficState::listToString(acs)  << std::endl;
  }
  std::cout << "Altitude Resolution (up): " << num2str(daa.altitudeResolution(true,"ft"),"ft")  << std::endl;
  std::cout << "Altitude Resolution (down): " << num2str(daa.altitudeResolution(false,"ft"),"ft")  << std::endl;
  std::cout << "Preferred Altitude Direction: ";
  if (daa.preferredAltitudeUpOrDown()) {
    std::cout << "up"  << std::endl;
  } else {
    std::cout << "down"  << std::endl;
  }
  std::cout << "Recovery Information for Altitude Bands:"  << std::endl;
  recovery = daa.altitudeRecoveryInformation();
  std::cout << "  Time to Recovery: " <<
      Units::str("s",recovery.timeToRecovery())  << std::endl;
  std::cout << "  Recovery Horizontal Distance: " <<
      Units::str("nmi",recovery.recoveryHorizontalDistance())  << std::endl;
  std::cout << "  Recovery Vertical Distance: " <<
      Units::str("ft",recovery.recoveryVerticalDistance())  << std::endl;
  std::cout << std::endl;

  // Last times to maneuver
  for (int ac_idx=1; ac_idx <= daa.lastTrafficIndex(); ++ac_idx) {
    TrafficState ac = daa.getAircraftStateAt(ac_idx);
    std::cout << "Last Times to Maneuver with Respect to " << ac.getId() << ":"  << std::endl;
    std::cout << "  " << hdstr << " Maneuver: " << num2str(daa.lastTimeToHorizontalDirectionManeuver(ac_idx),"s")  << std::endl;
    std::cout << "  " << hsstr << " Maneuver: " << num2str(daa.lastTimeToHorizontalSpeedManeuver(ac_idx),"s")  << std::endl;
    std::cout << "  Vertical Speed Maneuver: " << num2str(daa.lastTimeToVerticalSpeedManeuver(ac_idx),"s")  << std::endl;
    std::cout << "  Altitude Maneuver: " << num2str(daa.lastTimeToAltitudeManeuver(ac_idx),"s")  << std::endl;
  }
  std::cout << std::endl;
}

void printHorizontalContours(Daidalus& daa) {
  std::vector< std::vector<Position> > blobs;
  // Aircraft at index 0 is ownship
  for (int ac_idx=1; ac_idx <= daa.lastTrafficIndex(); ++ac_idx) {
    // Compute all contours
    daa.horizontalContours(blobs,ac_idx);
    std::vector< std::vector<Position> >::const_iterator blob_ptr;
    for (blob_ptr = blobs.begin(); blob_ptr != blobs.end(); ++blob_ptr) {
      std::cout << "Counter-clockwise Corrective Contour wrt Aircraft " << daa.getAircraftStateAt(ac_idx).getId() << ": " << std::endl;
      std::vector<Position>::const_iterator pos_ptr;
      for (pos_ptr = blob_ptr->begin(); pos_ptr != blob_ptr->end(); ++pos_ptr) {
        std::cout << pos_ptr->toString() << " ";
      }
      std::cout << std::endl;
    }
  }
}

void printHorizontalHazardZones(Daidalus& daa) {
  std::vector<Position> haz;
  // Aircraft at index 0 is ownship
  for (int ac_idx=1; ac_idx <= daa.lastTrafficIndex(); ++ac_idx) {
    // Compute hazard zone (violation)
    daa.horizontalHazardZone(haz,ac_idx,true,true); // Violation
    std::cout << "Counter-clockwise Corrective Hazard Zone wrt Aircraft " << daa.getAircraftStateAt(ac_idx).getId() << ": " << std::endl;
    std::vector<Position>::const_iterator pos_ptr;
    for (pos_ptr = haz.begin(); pos_ptr != haz.end(); ++pos_ptr) {
      std::cout << pos_ptr->toString() << " ";
    }
    std::cout << std::endl;
    // Compute hazard zone (conflict)
    daa.horizontalHazardZone(haz,ac_idx,false,true); // Conflict
    std::cout << "Counter-clockwise Corrective Hazard Zone (with alerting time) wrt Aircraft " << daa.getAircraftStateAt(ac_idx).getId() << ": " << std::endl;
    for (pos_ptr = haz.begin(); pos_ptr != haz.end(); ++pos_ptr) {
      std::cout << pos_ptr->toString() << " ";
    }
    std::cout << std::endl;
  }
}

//int main(int argc, char* argv[]) {
int main(int argc, char* argv[]){
  std::cout << "##" << std::endl;
  std::cout << "## " << Daidalus::release() << std::endl;
  std::cout << "##\n" << std::endl;
  bool verbose = false;
  // default test case
  // ifstream Intruder("/home/qgu4/RTCA DO-365 MOPS_Test_Cases/overtaking/O1/Intruder/O1_Truth_TVInt1.csv");
  // ifstream Ownship("/home/qgu4/RTCA DO-365 MOPS_Test_Cases/overtaking/O1/Ownship/O1_Truth_TVOwn.csv");
  // default LogFile
  // ofstream LogFile("logfiles/overtaking_O1.log");
  
  // Declare an empty Daidalus object
  Daidalus daa;
  
  std::string input_file = "";
  std::string output_file = "";
  std::string conf = "";

  // A Daidalus object can be configured either programmatically or by using a configuration file.
  for (int a=1;a < argc; ++a) {
    std::string arga = argv[a];
    if ((startsWith(arga,"--conf") || startsWith(arga,"-conf")) && a+1 < argc) {
      // Load configuration file
      arga = argv[++a];
      if (daa.loadFromFile(arga)) {
        std::cout << "Loading configuration file " << arga << std::endl;
      } else if (arga == "no_sum") {
        // Configure DAIDALUS as in DO-365B, without SUM
        daa.set_DO_365B(true,false);
      } else if (arga == "nom_a") {
        // Configure DAIDALUS to Nominal A: Buffered DWC, Kinematic Bands, Turn Rate 1.5 [deg/s]
        daa.set_Buffered_WC_DO_365(false);
      } else if (arga == "nom_b") {
        // Configure DAIDALUS to Nominal B: Buffered DWS, Kinematic Bands, Turn Rate 3.0 [deg/s]
        daa.set_Buffered_WC_DO_365(true);
      } else if (arga == "cd3d") {
        // Configure DAIDALUS to CD3D parameters: Cylinder (5nmi,1000ft), Instantaneous Bands, Only Corrective Volume
        daa.set_CD3D();
      } else if (arga == "tcasii") {
        // Configure DAIDALUS to ideal TCASII logic: TA is Preventive Volume and RA is Corrective One
        daa.set_TCASII();
      } else {
        std::cerr << "File " << arga << " not found" << std::endl;
        exit(1);
      }
    } else if (startsWith(arga,"--verb") || startsWith(arga,"-verb")) {
      verbose = true;
    } else if (startsWith(arga,"--h") || startsWith(arga,"-h")) {
      std::cerr << "Options:" << std::endl;
      std::cerr << "  --help\n\tPrint this message" << std::endl;
      std::cerr << "  --config <configuration-file> | no_sum | nom_a | nom_b | cd3d | tcasii\n\tLoad <configuration-file>" << std::endl;
      std::cerr << "  --verbose\n\tPrint more information" << std::endl;
      exit(0);
    } else if (startsWith(arga, "--testcase") || startsWith(arga, "-testcase")){
      arga = argv[++a];
      string base_filename = arga.substr(arga.find_last_of("/") + 1);
      string intruder_test = arga + "/" + "Intruder" + "/" + base_filename + "_ADSB_Tracker_TVInt1.csv";
      string ownship_test = arga + "/" + "Ownship" + "/" + base_filename + "_Tracker_TVOwn.csv";
      cout << "base_filename:" << base_filename << endl;
      cout << "intruder file:" << intruder_test << endl;
      cout << "ownship file:" << ownship_test << endl;
      ifstream Intruder(intruder_test); ifstream Ownship(ownship_test);
      ofstream LogFile("logfiles/" + base_filename + ".log");
      if (daa.numberOfAlerters()==0) {
        // If no alerter has been configured, configure alerters as in
        // DO_365B Phase I, Phase II, and Non-Cooperative, with SUM
        daa.set_DO_365B();
      }

      // initial parameters
      string ToA_intruder; string ICAO_intruder; string latitude_intruder; string longitude_intruder; string altitude_intruder; string VS_intruder; string Trk_intruder; string Gs_intruder;
      string ToA_ownship; string ICAO_ownship; string latitude_ownship; string longitude_ownship; string altitude_ownship; string VS_ownship; string Trk_ownship; string Gs_ownship;

      //initial start time
      getline(Intruder, ToA_intruder, ',');
      double t = stof(ToA_intruder);
      
      while (Intruder.good() && Ownship.good()){

        // for all times t (in this example, only one time step is illustrated)
        // Add ownship state at time t
        getline(Ownship, ToA_ownship, ',');
        getline(Ownship, ICAO_ownship, ',');
        getline(Ownship, latitude_ownship, ',');
        getline(Ownship, longitude_ownship, ',');
        getline(Ownship, altitude_ownship, ',');
        getline(Ownship, VS_ownship, ',');
        getline(Ownship, Trk_ownship, ',');
        getline(Ownship, Gs_ownship, '\n');

        if((int)ToA_ownship[0] >= 48 && (int)ToA_ownship[0] <= 57 && stof(ToA_ownship) == t){

                std::cout << "latitude_ownship:" << latitude_ownship << endl;
 	        std::cout << "longitude_ownship:" << longitude_ownship << endl;
	        std::cout << "altitude_ownship:" << altitude_ownship << endl;
	        std::cout << "VS_ownship:" << VS_ownship << endl;
	        std::cout << "Gs_ownship:" << Gs_ownship << endl;
	        std::cout << "Trk_ownship:" << Trk_ownship << endl;

	  Position so = Position::makeLatLonAlt(stof(latitude_ownship),"deg", stof(longitude_ownship),"deg", stof(altitude_ownship),"ft");
          Velocity vo = Velocity::makeTrkGsVs(stof(Trk_ownship),"deg", stof(Gs_ownship),"knot", stof(VS_ownship),"fpm");
          daa.setOwnshipState("ownship",so,vo,t);
        }
        else
          continue;
        
        // In case of multiple alerting logic (assuming ownship_centric is set to true), e.g.,
        int alerter_idx = 1;
        daa.setAlerterIndex(0,alerter_idx);

        // Add all traffic states at time t
        // ... some traffic ...
        getline(Intruder, ToA_intruder, ',');
        getline(Intruder, ICAO_intruder, ',');
        getline(Intruder, latitude_intruder, ',');
        getline(Intruder, longitude_intruder, ',');
        getline(Intruder, altitude_intruder, ',');
        getline(Intruder, VS_intruder, ',');
        getline(Intruder, Trk_intruder, ',');
        getline(Intruder, Gs_intruder, '\n');
        if((int)ToA_intruder[0] >= 48 && (int)ToA_intruder[0] <= 57){
          Position si = Position::makeLatLonAlt(stof(latitude_intruder),"deg", stof(longitude_intruder),"deg", stof(altitude_intruder),"ft");
          Velocity vi = Velocity::makeTrkGsVs(stof(Trk_intruder),"deg", stof(Gs_intruder),"knot", stof(VS_intruder),"fpm");
          int ac_idx = daa.addTrafficState("intruder",si,vi);
	
	  int alter_idx = 1;
          daa.setAlerterIndex(ac_idx,alter_idx);
        }

        // After all traffic has been added ...

        // Set wind vector (TO direction)
        Velocity wind = Velocity::makeTrkGsVs(45,"deg", 10,"knot", 0,"fpm");
        daa.setWindVelocityTo(wind);

        // Print Daidalus Object
        if (verbose) {
          std::cout << daa.toString() << std::endl;
        }

        // Print information about the Daidalus Object
        std::cout << "Number of Aircraft: " << daa.numberOfAircraft() << std::endl;
        std::cout << "Last Aircraft Index: " << daa.lastTrafficIndex() << std::endl;
        std::cout <<  std::endl;

        // Detect conflicts with every traffic aircraft
        printDetection(daa);

        // Call alerting logic for each traffic aircraft.
        printAlerts(daa);

        // Print bands information
        printBands(daa);

        if (verbose) {
          // Print horizontal contours (for display purposes only)
          printHorizontalContours(daa);

          // Print horizontal protected areas (for display purposes only)
          printHorizontalHazardZones(daa);
        }

        LogFile << daa.toString();
        // go to next time step
        t += 1.0f;
      }

      // after all time traffic calculated done, close the LogFile
      LogFile.close();
    }
    else {
      std::cerr << "Unknown option " << arga << std::endl;
      exit(0);
    }
  } 
}
