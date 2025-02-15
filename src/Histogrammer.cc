#include "Histogrammer.hh"

ISSHistogrammer::ISSHistogrammer( ISSReaction *myreact, ISSSettings *myset ){
	
	react = myreact;
	set = myset;
	
	// No progress bar by default
	_prog_ = false;
	
}

void ISSHistogrammer::MakeHists() {
	
    std::string hname, htitle;
    std::string dirname;
   
    std::vector<double> zbins;
   
    for ( int row = 0; row < 4; row++ ){
 
        for ( int ch = 0; ch <= 128; ch++ ){

            double d = 128 - ch;        // take the far end (low-edge) of the end strip
            d *= 0.953;                 // p-side strip pitch = 0.953 mm
            d += 1.508;                 // distance from wafer edge to active region
            d += 125.5 * (3.0 - row);   // move to correct row (125.0 mm wafer length + 0.5 mm inter-wafer gap)
            d *= -1.0;
            d += react->GetArrayDistance();

	    // Add a bin to the start and the end for space.
            if ( ch == 0 && row == 0 ){ 
                zbins.push_back( d - 10 );
            }
            
            zbins.push_back(d);
            
            if ( ch == 128 && row == 3 ){
                zbins.push_back( d + 10 );
            }
 
        } // ch
 
    } // row
	
	// Array physics histograms
	// Singles mode
	dirname = "SinglesMode";
	output_file->mkdir( dirname.data() );
	output_file->cd( dirname.data() );
	
	hname = "E_vs_z";
	htitle = "Energy vs. z distance;z [mm];Energy [keV];Counts per mm per 20 keV";
	E_vs_z = new TH2F( hname.data(), htitle.data(), zbin.size()-1, zbins.data(), 800, 0, 16000 );
	
	hname = "Ex";
	htitle = "Excitation energy;Excitation energy [keV];Counts per 20 keV";
	Ex = new TH1F( hname.data(), htitle.data(), 850, -2000, 15000 );
	
	hname = "Ex_vs_theta";
	htitle = "Excitation energy vs. centre of mass angle;#theta_{CM} [deg.];Excitation energy [keV];Counts per deg per 20 keV";
	Ex_vs_theta = new TH2F( hname.data(), htitle.data(), 180, 0, 180.0, 800, -1000, 15000 );
	
	hname = "Ex_vs_z";
	htitle = "Excitation energy vs. measured z;z [mm];Excitation energy [keV];Counts per mm per 20 keV";
	Ex_vs_z = new TH2F( hname.data(), htitle.data(), zbin.size()-1, zbins.data(), 850, -2000, 15000 );
	
	// For each user cut
	E_vs_z_cut.resize( react->GetNumberOfEvsZCuts() );
	Ex_cut.resize( react->GetNumberOfEvsZCuts() );
	Ex_vs_theta_cut.resize( react->GetNumberOfEvsZCuts() );
	Ex_vs_z_cut.resize( react->GetNumberOfEvsZCuts() );
	for( unsigned int j = 0; j < react->GetNumberOfEvsZCuts(); ++j ) {
		
		dirname = "SinglesMode/cut_" + std::to_string(j);
		output_file->mkdir( dirname.data() );
		output_file->cd( dirname.data() );
		
		hname = "E_vs_z_cut" + std::to_string(j);
		htitle = "Energy vs. z distance for user cut " + std::to_string(j);
		htitle += ";z [mm];Energy [keV];Counts per mm per 20 keV";
		E_vs_z_cut[j] = new TH2F( hname.data(), htitle.data(), zbin.size()-1, zbins.data(), 800, 0, 16000 );
		
		hname = "Ex_cut" + std::to_string(j);
		htitle = "Excitation energy for user cut " + std::to_string(j);
		htitle += ";Excitation energy [keV];Counts per 20 keV";
		Ex_cut[j] = new TH1F( hname.data(), htitle.data(), 850, -2000, 15000 );
		
		hname = "Ex_vs_theta_cut" + std::to_string(j);
		htitle = "Excitation energy vs. centre of mass angle for user cut " + std::to_string(j);
		htitle += ";#theta_{CM} [deg];Excitation energy [keV];Counts per deg per 20 keV";
		Ex_vs_theta_cut[j] = new TH2F( hname.data(), htitle.data(), 180, 0, 180.0, 850, -2000, 15000 );
		
		hname = "Ex_vs_z_cut" + std::to_string(j);
		htitle = "Excitation energy vs. centre of mass angle for user cut " + std::to_string(j);
		htitle += ";z [mm];Excitation energy [keV];Counts per mm per 20 keV";
		Ex_vs_z_cut[j] = new TH2F( hname.data(), htitle.data(), zbin.size()-1, zbins.data(), 850, -2000, 15000 );
		
		
	}
	
	// For each array module
	E_vs_z_mod.resize( set->GetNumberOfArrayModules() );
	Ex_mod.resize( set->GetNumberOfArrayModules() );
	Ex_vs_theta_mod.resize( set->GetNumberOfArrayModules() );
	Ex_vs_z_mod.resize( set->GetNumberOfArrayModules() );
	for( unsigned int j = 0; j < set->GetNumberOfArrayModules(); ++j ) {
		
		dirname = "SinglesMode/module_" + std::to_string(j);
		output_file->mkdir( dirname.data() );
		output_file->cd( dirname.data() );
		
		E_vs_z_mod.resize( set->GetNumberOfArrayModules() );
		hname = "E_vs_z_mod" + std::to_string(j);
		htitle = "Energy vs. z distance for module " + std::to_string(j);
		htitle += ";z [mm];Energy [keV];Counts per mm per 20 keV";
		E_vs_z_mod[j] = new TH2F( hname.data(), htitle.data(), zbin.size()-1, zbins.data(), 800, 0, 16000 );
		
		hname = "Ex_mod" + std::to_string(j);
		htitle = "Excitation energy for module " + std::to_string(j);
		htitle += ";Excitation energy [keV];Counts per 20 keV";
		Ex_mod[j] = new TH1F( hname.data(), htitle.data(), 850, -2000, 15000 );
		
		hname = "Ex_vs_theta_mod" + std::to_string(j);
		htitle = "Excitation energy vs. centre of mass angle for module " + std::to_string(j);
		htitle += ";#theta_{CM} [deg];Excitation energy [keV];Counts per deg per 20 keV";
		Ex_vs_theta_mod[j] = new TH2F( hname.data(), htitle.data(), 180, 0, 180.0, 850, -2000, 15000 );
		
		hname = "Ex_vs_z_mod" + std::to_string(j);
		htitle = "Excitation energy vs. centre of mass angle for module " + std::to_string(j);
		htitle += ";z [mm];Excitation energy [keV];Counts per mm per 20 keV";
		Ex_vs_z_mod[j] = new TH2F( hname.data(), htitle.data(), zbin.size()-1, zbins.data(),  850, -2000, 15000 );
		
	}
	
	// EBIS mode
	dirname = "EBISMode";
	output_file->mkdir( dirname.data() );
	output_file->cd( dirname.data() );
	
	hname = "E_vs_z_ebis";
	htitle = "Energy vs. z distance gated on EBIS and off beam subtracted;z [mm];Energy [keV];Counts per mm per 20 keV";
	E_vs_z_ebis = new TH2F( hname.data(), htitle.data(), zbin.size()-1, zbins.data(), 800, 0, 16000 );
	
	hname = "E_vs_z_ebis_on";
	htitle = "Energy vs. z distance gated on EBIS;z [mm];Energy [keV];Counts per mm per 20 keV";
	E_vs_z_ebis_on = new TH2F( hname.data(), htitle.data(), zbin.size()-1, zbins.data(), 800, 0, 16000 );
	
	hname = "E_vs_z_ebis_off";
	htitle = "Energy vs. z distance gated off EBIS;z [mm];Energy [keV];Counts per mm per 20 keV";
	E_vs_z_ebis_off = new TH2F( hname.data(), htitle.data(), zbin.size()-1, zbins.data(), 800, 0, 16000 );
	
	hname = "Ex_ebis";
	htitle = "Excitation energy gated by EBIS and off beam subtracted;Excitation energy [keV];Counts per 20 keV";
	Ex_ebis = new TH1F( hname.data(), htitle.data(), 850, -2000, 15000 );
	
	hname = "Ex_ebis_on";
	htitle = "Excitation energy gated on EBIS;Excitation energy [keV];Counts per 20 keV";
	Ex_ebis_on = new TH1F( hname.data(), htitle.data(), 850, -2000, 15000 );
	
	hname = "Ex_ebis_off";
	htitle = "Excitation energy gated off EBIS;Excitation energy [keV];Counts per 20 keV";
	Ex_ebis_off = new TH1F( hname.data(), htitle.data(), 850, -2000, 15000 );
	
	hname = "Ex_vs_theta_ebis";
	htitle = "Excitation energy vs. centre of mass angle gated by EBIS and off beam subtracted;#theta_{CM} [deg];Excitation energy [keV];Counts per deg per 20 keV";
	Ex_vs_theta_ebis = new TH2F( hname.data(), htitle.data(), 180, 0, 180.0, 800, -1000, 15000 );
	
	hname = "Ex_vs_theta_ebis_on";
	htitle = "Excitation energy vs. centre of mass angle gated on EBIS;#theta_{CM} [deg];Excitation energy [keV];Counts per deg per 20 keV";
	Ex_vs_theta_ebis_on = new TH2F( hname.data(), htitle.data(), 180, 0, 180.0, 800, -1000, 15000 );
	
	hname = "Ex_vs_theta_ebis_off";
	htitle = "Excitation energy vs. centre of mass angle gated off EBIS;#theta_{CM} [deg];Excitation energy [keV];Counts per deg per 20 keV";
	Ex_vs_theta_ebis_off = new TH2F( hname.data(), htitle.data(), 180, 0, 180.0, 800, -1000, 15000 );
	
	hname = "Ex_vs_z_ebis";
	htitle = "Excitation energy vs. measured z gated by EBIS and off beam subtracted;z [mm];Excitation energy [keV];Counts per mm per 20 keV";
	Ex_vs_z_ebis = new TH2F( hname.data(), htitle.data(), zbin.size()-1, zbins.data(),  850, -2000, 15000 );
	
	hname = "Ex_vs_z_ebis_on";
	htitle = "Excitation energy vs. measured z gated on EBIS;z [mm];Excitation energy [keV];Counts per mm per 20 keV";
	Ex_vs_z_ebis_on = new TH2F( hname.data(), htitle.data(), zbin.size()-1, zbins.data(), 850, -2000, 15000 );
	
	hname = "Ex_vs_z_ebis_off";
	htitle = "Excitation energy vs. measured z gated off EBIS;z [mm];Excitation energy [keV];Counts per mm per 20 keV";
	Ex_vs_z_ebis_off = new TH2F( hname.data(), htitle.data(), zbin.size()-1, zbins.data(), 850, -2000, 15000 );
	
	// For each user cut
	E_vs_z_ebis_cut.resize( react->GetNumberOfEvsZCuts() );
	E_vs_z_ebis_on_cut.resize( react->GetNumberOfEvsZCuts() );
	E_vs_z_ebis_off_cut.resize( react->GetNumberOfEvsZCuts() );
	Ex_ebis_cut.resize( react->GetNumberOfEvsZCuts() );
	Ex_ebis_on_cut.resize( react->GetNumberOfEvsZCuts() );
	Ex_ebis_off_cut.resize( react->GetNumberOfEvsZCuts() );
	Ex_vs_theta_ebis_cut.resize( react->GetNumberOfEvsZCuts() );
	Ex_vs_theta_ebis_on_cut.resize( react->GetNumberOfEvsZCuts() );
	Ex_vs_theta_ebis_off_cut.resize( react->GetNumberOfEvsZCuts() );
	Ex_vs_z_ebis_cut.resize( react->GetNumberOfEvsZCuts() );
	Ex_vs_z_ebis_on_cut.resize( react->GetNumberOfEvsZCuts() );
	Ex_vs_z_ebis_off_cut.resize( react->GetNumberOfEvsZCuts() );
	for( unsigned int j = 0; j < react->GetNumberOfEvsZCuts(); ++j ) {
		
		dirname = "EBISMode/cut_" + std::to_string(j);
		output_file->mkdir( dirname.data() );
		output_file->cd( dirname.data() );
		
		hname = "E_vs_z_ebis_cut" + std::to_string(j);
		htitle = "Energy vs. z distance for user cut " + std::to_string(j);
		htitle += " gated by EBIS and off beam subtracted;z [mm];Energy [keV];Counts per mm per 20 keV";
		E_vs_z_ebis_cut[j] = new TH2F( hname.data(), htitle.data(), zbin.size()-1, zbins.data(), 800, 0, 16000 );
		
		hname = "E_vs_z_ebis_on_cut" + std::to_string(j);
		htitle = "Energy vs. z distance for user cut " + std::to_string(j);
		htitle += " gated on EBIS;z [mm];Energy [keV];Counts per mm per 20 keV";
		E_vs_z_ebis_on_cut[j] = new TH2F( hname.data(), htitle.data(), zbin.size()-1, zbins.data(), 800, 0, 16000 );
		
		hname = "E_vs_z_ebis_off_cut" + std::to_string(j);
		htitle = "Energy vs. z distance for user cut " + std::to_string(j);
		htitle += " gated off EBIS;z [mm];Energy [keV];Counts per mm per 20 keV";
		E_vs_z_ebis_off_cut[j] = new TH2F( hname.data(), htitle.data(), zbin.size()-1, zbins.data(), 800, 0, 16000 );
		
		hname = "Ex_ebis_cut" + std::to_string(j);
		htitle = "Excitation energy for user cut " + std::to_string(j);
		htitle += " gated by EBIS and off beam subtracted;Excitation energy [keV];Counts per mm per 20 keV";
		Ex_ebis_cut[j] = new TH1F( hname.data(), htitle.data(), 850, -2000, 15000 );
		
		hname = "Ex_ebis_on_cut" + std::to_string(j);
		htitle = "Excitation energy for user cut " + std::to_string(j);
		htitle += " gated on EBIS;Excitation energy [keV];Counts per 20 keV";
		Ex_ebis_on_cut[j] = new TH1F( hname.data(), htitle.data(), 850, -2000, 15000 );
		
		hname = "Ex_ebis_off_cut" + std::to_string(j);
		htitle = "Excitation energy for user cut " + std::to_string(j);
		htitle += " gated off EBIS;Excitation energy [keV];Counts per 20 keV";
		Ex_ebis_off_cut[j] = new TH1F( hname.data(), htitle.data(), 850, -2000, 15000 );
		
		hname = "Ex_vs_theta_ebis_cut" + std::to_string(j);
		htitle = "Excitation energy vs. centre of mass angle for user cut " + std::to_string(j);
		htitle += " gated by EBIS and off beam subtracted;#theta_{CM} [deg];Excitation energy [keV];Counts per deg per 20 keV";
		Ex_vs_theta_ebis_cut[j] = new TH2F( hname.data(), htitle.data(), 180, 0, 180.0, 850, -2000, 15000 );
		
		hname = "Ex_vs_theta_ebis_on_cut" + std::to_string(j);
		htitle = "Excitation energy vs. centre of mass angle for user cut " + std::to_string(j);
		htitle += " gated by EBIS and off beam subtracted;#theta_{CM} [deg];Excitation energy [keV];Counts per deg per 20 keV";
		Ex_vs_theta_ebis_on_cut[j] = new TH2F( hname.data(), htitle.data(), 180, 0, 180.0, 850, -2000, 15000 );
		
		hname = "Ex_vs_theta_ebis_off_cut" + std::to_string(j);
		htitle = "Excitation energy vs. centre of mass angle for user cut " + std::to_string(j);
		htitle += " gated by EBIS and off beam subtracted;#theta_{CM} [deg];Excitation energy [keV];Counts per deg per 20 keV";
		Ex_vs_theta_ebis_off_cut[j] = new TH2F( hname.data(), htitle.data(), 180, 0, 180.0, 850, -2000, 15000 );
		
		hname = "Ex_vs_z_ebis_cut" + std::to_string(j);
		htitle = "Excitation energy vs. measured z for user cut " + std::to_string(j);
		htitle += " gated by EBIS and off beam subtracted;z [mm];Excitation energy [keV];Counts per mm per 20 keV";
		Ex_vs_z_ebis_cut[j] = new TH2F( hname.data(), htitle.data(), zbin.size()-1, zbins.data(), 850, -2000, 15000 );
		
		hname = "Ex_vs_z_ebis_on_cut" + std::to_string(j);
		htitle = "Excitation energy vs. measured z  for user cut " + std::to_string(j);
		htitle += " gated by EBIS and off beam subtracted;z [mm];Excitation energy [keV];Counts per mm per 20 keV";
		Ex_vs_z_ebis_on_cut[j] = new TH2F( hname.data(), htitle.data(), zbin.size()-1, zbins.data(), 850, -2000, 15000 );
		
		hname = "Ex_vs_z_ebis_off_cut" + std::to_string(j);
		htitle = "Excitation energy vs. measured z  for user cut " + std::to_string(j);
		htitle += " gated by EBIS and off beam subtracted;z [mm];Excitation energy [keV];Counts per mm per 20 keV";
		Ex_vs_z_ebis_off_cut[j] = new TH2F( hname.data(), htitle.data(), zbin.size()-1, zbins.data(), 850, -2000, 15000 );
		
	}
	
	// For each array module
	E_vs_z_ebis_mod.resize( set->GetNumberOfArrayModules() );
	E_vs_z_ebis_on_mod.resize( set->GetNumberOfArrayModules() );
	E_vs_z_ebis_off_mod.resize( set->GetNumberOfArrayModules() );
	Ex_ebis_mod.resize( set->GetNumberOfArrayModules() );
	Ex_ebis_on_mod.resize( set->GetNumberOfArrayModules() );
	Ex_ebis_off_mod.resize( set->GetNumberOfArrayModules() );
	Ex_vs_theta_ebis_mod.resize( set->GetNumberOfArrayModules() );
	Ex_vs_theta_ebis_on_mod.resize( set->GetNumberOfArrayModules() );
	Ex_vs_theta_ebis_off_mod.resize( set->GetNumberOfArrayModules() );
	Ex_vs_z_ebis_mod.resize( set->GetNumberOfArrayModules() );
	Ex_vs_z_ebis_on_mod.resize( set->GetNumberOfArrayModules() );
	Ex_vs_z_ebis_off_mod.resize( set->GetNumberOfArrayModules() );
	for( unsigned int j = 0; j < set->GetNumberOfArrayModules(); ++j ) {
		
		dirname = "EBISMode/module_" + std::to_string(j);
		output_file->mkdir( dirname.data() );
		output_file->cd( dirname.data() );
		
		hname = "E_vs_z_ebis_mod" + std::to_string(j);
		htitle = "Energy vs. z distance for module " + std::to_string(j);
		htitle += " gated by EBIS and off beam subtracted;z [mm];Energy [keV];Counts per mm per 20 keV";
		E_vs_z_ebis_mod[j] = new TH2F( hname.data(), htitle.data(), zbin.size()-1, zbins.data(), 800, 0, 16000 );
		
		hname = "E_vs_z_ebis_on_mod" + std::to_string(j);
		htitle = "Energy vs. z distance for module " + std::to_string(j);
		htitle += " gated on EBIS;z [mm];Energy [keV];Counts per mm per 20 keV";
		E_vs_z_ebis_on_mod[j] = new TH2F( hname.data(), htitle.data(), zbin.size()-1, zbins.data(), 800, 0, 16000 );
		
		hname = "E_vs_z_ebis_off_mod" + std::to_string(j);
		htitle = "Energy vs. z distance for module " + std::to_string(j);
		htitle += " gated off EBIS;z [mm];Energy [keV];Counts per mm per 20 keV";
		E_vs_z_ebis_off_mod[j] = new TH2F( hname.data(), htitle.data(), zbin.size()-1, zbins.data(), 800, 0, 16000 );
		
		hname = "Ex_ebis_mod" + std::to_string(j);
		htitle = "Excitation energy for module " + std::to_string(j);
		htitle += " gated by EBIS and off beam subtracted;Excitation energy [keV];Counts per mm per 20 keV";
		Ex_ebis_mod[j] = new TH1F( hname.data(), htitle.data(), 850, -2000, 15000 );
		
		hname = "Ex_ebis_on_mod" + std::to_string(j);
		htitle = "Excitation energy for module " + std::to_string(j);
		htitle += " gated on EBIS;Excitation energy [keV];Counts per 20 keV";
		Ex_ebis_on_mod[j] = new TH1F( hname.data(), htitle.data(), 850, -2000, 15000 );
		
		hname = "Ex_ebis_off_mod" + std::to_string(j);
		htitle = "Excitation energy for module " + std::to_string(j);
		htitle += " gated off EBIS;Excitation energy [keV];Counts per 20 keV";
		Ex_ebis_off_mod[j] = new TH1F( hname.data(), htitle.data(), 850, -2000, 15000 );
		
		hname = "Ex_vs_theta_ebis_mod" + std::to_string(j);
		htitle = "Excitation energy vs. centre of mass angle for module " + std::to_string(j);
		htitle += " gated by EBIS and off beam subtracted;#theta_{CM} [deg];Excitation energy [keV];Counts per deg per 20 keV";
		Ex_vs_theta_ebis_mod[j] = new TH2F( hname.data(), htitle.data(), 180, 0, 180.0, 850, -2000, 15000 );
		
		hname = "Ex_vs_theta_ebis_on_mod" + std::to_string(j);
		htitle = "Excitation energy vs. centre of mass angle for module " + std::to_string(j);
		htitle += " gated by EBIS and off beam subtracted;#theta_{CM} [deg];Excitation energy [keV];Counts per deg per 20 keV";
		Ex_vs_theta_ebis_on_mod[j] = new TH2F( hname.data(), htitle.data(), 180, 0, 180.0, 850, -2000, 15000 );
		
		hname = "Ex_vs_theta_ebis_off_mod" + std::to_string(j);
		htitle = "Excitation energy vs. centre of mass angle for module " + std::to_string(j);
		htitle += " gated by EBIS and off beam subtracted;#theta_{CM} [deg];Excitation energy [keV];Counts per deg per 20 keV";
		Ex_vs_theta_ebis_off_mod[j] = new TH2F( hname.data(), htitle.data(), 180, 0, 180.0, 850, -2000, 15000 );
		
		hname = "Ex_vs_z_ebis_mod" + std::to_string(j);
		htitle = "Excitation energy vs. measured z for module " + std::to_string(j);
		htitle += " gated by EBIS and off beam subtracted;z [mm];Excitation energy [keV];Counts per mm per 20 keV";
		Ex_vs_z_ebis_mod[j] = new TH2F( hname.data(), htitle.data(), zbin.size()-1, zbins.data(), 850, -2000, 15000 );
		
		hname = "Ex_vs_z_ebis_on_mod" + std::to_string(j);
		htitle = "Excitation energy vs. measured z  for module " + std::to_string(j);
		htitle += " gated by EBIS and off beam subtracted;z [mm];Excitation energy [keV];Counts per mm per 20 keV";
		Ex_vs_z_ebis_on_mod[j] = new TH2F( hname.data(), htitle.data(), zbin.size()-1, zbins.data(), 850, -2000, 15000 );
		
		hname = "Ex_vs_z_ebis_off_mod" + std::to_string(j);
		htitle = "Excitation energy vs. measured z  for module " + std::to_string(j);
		htitle += " gated by EBIS and off beam subtracted;z [mm];Excitation energy [keV];Counts per mm per 20 keV";
		Ex_vs_z_ebis_off_mod[j] = new TH2F( hname.data(), htitle.data(), zbin.size()-1, zbins.data(), 850, -2000, 15000 );
		
	}
	
	// Recoil mode
	dirname = "RecoilMode";
	output_file->mkdir( dirname.data() );
	output_file->cd( dirname.data() );
	
	hname = "E_vs_z_recoil";
	htitle = "Energy vs. z distance gated on recoils;z [mm];Energy [keV];Counts per mm per 20 keV";
	E_vs_z_recoil = new TH2F( hname.data(), htitle.data(), zbin.size()-1, zbins.data(), 800, 0, 16000 );
	
	hname = "E_vs_z_recoilT";
	htitle = "Energy vs. z distance with a time gate on recoils;z [mm];Energy [keV];Counts per mm per 20 keV";
	E_vs_z_recoilT = new TH2F( hname.data(), htitle.data(), zbin.size()-1, zbins.data(), 800, 0, 16000 );
	
	hname = "Ex_recoil";
	htitle = "Excitation energy gated by recoils;Excitation energy [keV];Counts per 20 keV";
	Ex_recoil = new TH1F( hname.data(), htitle.data(), 800, -1000, 15000 );
	
	hname = "Ex_recoilT";
	htitle = "Excitation energy with a time gate on all recoils;Excitation energy [keV];Counts per 20 keV";
	Ex_recoilT = new TH1F( hname.data(), htitle.data(), 800, -1000, 15000 );
	
	hname = "Ex_vs_theta_recoil";
	htitle = "Excitation energy vs. centre of mass angle gated by recoils;#theta_{CM} [deg];Excitation energy [keV];Counts per deg per 20 keV";
	Ex_vs_theta_recoil = new TH2F( hname.data(), htitle.data(), 180, 0, 180.0, 800, -1000, 15000 );
	
	hname = "Ex_vs_theta_recoilT";
	htitle = "Excitation energy vs. centre of mass angle with a time gate on all recoils;#theta_{CM} [deg];Excitation energy [keV];Counts per deg per 20 keV";
	Ex_vs_theta_recoilT = new TH2F( hname.data(), htitle.data(), 180, 0, 180.0, 800, -1000, 15000 );
	
	hname = "Ex_vs_z_recoil";
	htitle = "Excitation energy vs. measured z gated by recoils;z [mm];Excitation energy [keV];Counts per mm per 20 keV";
	Ex_vs_z_recoil = new TH2F( hname.data(), htitle.data(), zbin.size()-1, zbins.data(), 800, -1000, 15000 );
	
	hname = "Ex_vs_z_recoilT";
	htitle = "Excitation energy vs. measured z with a time gate on all recoils;z [mm];Excitation energy [keV];Counts per mm per 20 keV";
	Ex_vs_z_recoilT = new TH2F( hname.data(), htitle.data(), zbin.size()-1, zbins.data(), 800, -1000, 15000 );
	
	// For each user cut
	E_vs_z_recoil_cut.resize( react->GetNumberOfEvsZCuts() );
	E_vs_z_recoilT_cut.resize( react->GetNumberOfEvsZCuts() );
	Ex_recoil_cut.resize( react->GetNumberOfEvsZCuts() );
	Ex_recoilT_cut.resize( react->GetNumberOfEvsZCuts() );
	Ex_vs_theta_recoil_cut.resize( react->GetNumberOfEvsZCuts() );
	Ex_vs_theta_recoilT_cut.resize( react->GetNumberOfEvsZCuts() );
	Ex_vs_z_recoil_cut.resize( react->GetNumberOfEvsZCuts() );
	Ex_vs_z_recoilT_cut.resize( react->GetNumberOfEvsZCuts() );
	for( unsigned int j = 0; j < react->GetNumberOfEvsZCuts(); ++j ) {
		
		dirname = "RecoilMode/cut_" + std::to_string(j);
		output_file->mkdir( dirname.data() );
		output_file->cd( dirname.data() );
		
		hname = "E_vs_z_recoil_cut" + std::to_string(j);
		htitle = "Energy vs. z distance for user cut " + std::to_string(j);
		htitle += " gated on recoils;z [mm];Energy [keV];Counts per mm per 20 keV";
		E_vs_z_recoil_cut[j] = new TH2F( hname.data(), htitle.data(), zbin.size()-1, zbins.data(), 800, 0, 16000 );
		
		hname = "E_vs_z_recoilT_cut" + std::to_string(j);
		htitle = "Energy vs. z distance for user cut " + std::to_string(j);
		htitle += " with a time gate on all recoils;z [mm];Energy [keV];Counts per mm per 20 keV";
		E_vs_z_recoilT_cut[j] = new TH2F( hname.data(), htitle.data(), zbin.size()-1, zbins.data(), 800, 0, 16000 );
		
		hname = "Ex_recoil_cut" + std::to_string(j);
		htitle = "Excitation energy for user cut " + std::to_string(j);
		htitle += " gated by recoils;Excitation energy [keV];Counts per 20 keV";
		Ex_recoil_cut[j] = new TH1F( hname.data(), htitle.data(), 850, -2000, 15000 );
		
		hname = "Ex_recoilT_cut" + std::to_string(j);
		htitle = "Excitation energy for user cut " + std::to_string(j);
		htitle += " with a time gate on all recoils;Excitation energy [keV];Counts per 20 keV";
		Ex_recoilT_cut[j] = new TH1F( hname.data(), htitle.data(), 850, -2000, 15000 );
		
		hname = "Ex_vs_theta_recoil_cut" + std::to_string(j);
		htitle = "Excitation energy vs. centre of mass angle for user cut " + std::to_string(j);
		htitle += " gated by recoils;#theta_{CM} [deg];Excitation energy [keV];Counts per deg per 20 keV";
		Ex_vs_theta_recoil_cut[j] = new TH2F( hname.data(), htitle.data(), 180, 0, 180.0, 850, -2000, 15000 );
		
		hname = "Ex_vs_theta_recoilT_cut" + std::to_string(j);
		htitle = "Excitation energy vs. centre of mass angle for user cut " + std::to_string(j);
		htitle += " with a time gate on all recoils;#theta_{CM} [deg];Excitation energy [keV];Counts per deg per 20 keV";
		Ex_vs_theta_recoilT_cut[j] = new TH2F( hname.data(), htitle.data(), 180, 0, 180.0, 850, -2000, 15000 );
		
		hname = "Ex_vs_z_recoil_cut" + std::to_string(j);
		htitle = "Excitation energy vs. measured z for user cut " + std::to_string(j);
		htitle += " gated by recoils;z [mm];Excitation energy [keV];Counts per mm per 20 keV";
		Ex_vs_z_recoil_cut[j] = new TH2F( hname.data(), htitle.data(), zbin.size()-1, zbins.data(), 850, -2000, 15000 );
		
		hname = "Ex_vs_z_recoilT_cut" + std::to_string(j);
		htitle = "Excitation energy vs. measured z for user cut " + std::to_string(j);
		htitle += " with a time gate on all recoils;z [mm];Excitation energy [keV];Counts per mm per 20 keV";
		Ex_vs_z_recoilT_cut[j] = new TH2F( hname.data(), htitle.data(), zbin.size()-1, zbins.data(), 850, -2000, 15000 );
		
	} // Array
	
	// For each array module
	E_vs_z_recoil_mod.resize( set->GetNumberOfArrayModules() );
	E_vs_z_recoilT_mod.resize( set->GetNumberOfArrayModules() );
	Ex_recoil_mod.resize( set->GetNumberOfArrayModules() );
	Ex_recoilT_mod.resize( set->GetNumberOfArrayModules() );
	Ex_vs_theta_recoil_mod.resize( set->GetNumberOfArrayModules() );
	Ex_vs_theta_recoilT_mod.resize( set->GetNumberOfArrayModules() );
	Ex_vs_z_recoil_mod.resize( set->GetNumberOfArrayModules() );
	Ex_vs_z_recoilT_mod.resize( set->GetNumberOfArrayModules() );
	for( unsigned int j = 0; j < set->GetNumberOfArrayModules(); ++j ) {
		
		dirname = "RecoilMode/module_" + std::to_string(j);
		output_file->mkdir( dirname.data() );
		output_file->cd( dirname.data() );
		
		hname = "E_vs_z_recoil_mod" + std::to_string(j);
		htitle = "Energy vs. z distance for module " + std::to_string(j);
		htitle += " gated on recoils;z [mm];Energy [keV];Counts per mm per 20 keV";
		E_vs_z_recoil_mod[j] = new TH2F( hname.data(), htitle.data(), zbin.size()-1, zbins.data(), 800, 0, 16000 );
		
		hname = "E_vs_z_recoilT_mod" + std::to_string(j);
		htitle = "Energy vs. z distance for module " + std::to_string(j);
		htitle += " with a time gate on all recoils;z [mm];Energy [keV];Counts per mm per 20 keV";
		E_vs_z_recoilT_mod[j] = new TH2F( hname.data(), htitle.data(), zbin.size()-1, zbins.data(), 800, 0, 16000 );
		
		hname = "Ex_recoil_mod" + std::to_string(j);
		htitle = "Excitation energy for module " + std::to_string(j);
		htitle += " gated by recoils;Excitation energy [keV];Counts per 20 keV";
		Ex_recoil_mod[j] = new TH1F( hname.data(), htitle.data(), 850, -2000, 15000 );
		
		hname = "Ex_recoilT_mod" + std::to_string(j);
		htitle = "Excitation energy for module " + std::to_string(j);
		htitle += " with a time gate on all recoils;Excitation energy [keV];Counts per 20 keV";
		Ex_recoilT_mod[j] = new TH1F( hname.data(), htitle.data(), 850, -2000, 15000 );
		
		hname = "Ex_vs_theta_recoil_mod" + std::to_string(j);
		htitle = "Excitation energy vs. centre of mass angle for module " + std::to_string(j);
		htitle += " gated by recoils;#theta_{CM} [deg];Excitation energy [keV];Counts per deg per 20 keV";
		Ex_vs_theta_recoil_mod[j] = new TH2F( hname.data(), htitle.data(), 180, 0, 180.0, 850, -2000, 15000 );
		
		hname = "Ex_vs_theta_recoilT_mod" + std::to_string(j);
		htitle = "Excitation energy vs. centre of mass angle for module " + std::to_string(j);
		htitle += " with a time gate on all recoils;#theta_{CM} [deg];Excitation energy [keV];Counts per deg per 20 keV";
		Ex_vs_theta_recoilT_mod[j] = new TH2F( hname.data(), htitle.data(), 180, 0, 180.0, 850, -2000, 15000 );
		
		hname = "Ex_vs_z_recoil_mod" + std::to_string(j);
		htitle = "Excitation energy vs. measured z for module " + std::to_string(j);
		htitle += " gated by recoils;z [mm];Excitation energy [keV];Counts per mm per 20 keV";
		Ex_vs_z_recoil_mod[j] = new TH2F( hname.data(), htitle.data(), zbin.size()-1, zbins.data(), 850, -2000, 15000 );
		
		hname = "Ex_vs_z_recoilT_mod" + std::to_string(j);
		htitle = "Excitation energy vs. measured z for module " + std::to_string(j);
		htitle += " with a time gate on all recoils;z [mm];Excitation energy [keV];Counts per mm per 20 keV";
		Ex_vs_z_recoilT_mod[j] = new TH2F( hname.data(), htitle.data(), zbin.size()-1, zbins.data(), 850, -2000, 15000 );
		
	} // Array
	
	
	// T1 mode
	dirname = "T1Mode";
	output_file->mkdir( dirname.data() );
	output_file->cd( dirname.data() );
	
	hname = "E_vs_z_T1";
	htitle = "Energy vs. z distance with a time gate on T1 proton pulse;z [mm];Energy [keV];Counts per mm per 20 keV";
	E_vs_z_T1 = new TH2F( hname.data(), htitle.data(), zbin.size()-1, zbins.data(), 800, 0, 16000 );
	
	hname = "Ex_T1";
	htitle = "Excitation energy with a time gate on T1 proton pulse;Excitation energy [keV];Counts per 20 keV";
	Ex_T1 = new TH1F( hname.data(), htitle.data(), 800, -1000, 15000 );
	
	hname = "Ex_vs_T1";
	htitle = "Excitation energy as a function of time since T1 proton pulse;Event time - T1 [ns];Excitation energy [keV];Counts per 20 keV";
	Ex_vs_T1 = new TH2F( hname.data(), htitle.data(), 1000, 0, 100e9, 800, -1000, 15000 );
	
	hname = "Ex_vs_theta_T1";
	htitle = "Excitation energy vs. centre of mass angle with a time gate on T1 proton pulse;#theta_{CM} [deg];Excitation energy [keV];Counts per deg per 20 keV";
	Ex_vs_theta_T1 = new TH2F( hname.data(), htitle.data(), 180, 0, 180.0, 800, -1000, 15000 );
	
	hname = "Ex_vs_z_T1";
	htitle = "Excitation energy vs. measured z with a time gate on T1 proton pulse;z [mm];Excitation energy [keV];Counts per mm per 20 keV";
	Ex_vs_z_T1 = new TH2F( hname.data(), htitle.data(), zbin.size()-1, zbins.data(), 800, -1000, 15000 );
	
	// For each user cut
	E_vs_z_T1_cut.resize( react->GetNumberOfEvsZCuts() );
	Ex_T1_cut.resize( react->GetNumberOfEvsZCuts() );
	Ex_vs_T1_cut.resize( react->GetNumberOfEvsZCuts() );
	Ex_vs_theta_T1_cut.resize( react->GetNumberOfEvsZCuts() );
	Ex_vs_z_T1_cut.resize( react->GetNumberOfEvsZCuts() );
	for( unsigned int j = 0; j < react->GetNumberOfEvsZCuts(); ++j ) {
		
		dirname = "T1Mode/cut_" + std::to_string(j);
		output_file->mkdir( dirname.data() );
		output_file->cd( dirname.data() );
		
		hname = "E_vs_z_T1_cut" + std::to_string(j);
		htitle = "Energy vs. z distance for user cut " + std::to_string(j);
		htitle += " with a time gate on T1 proton pulse;z [mm];Energy [keV];Counts per mm per 20 keV";
		E_vs_z_T1_cut[j] = new TH2F( hname.data(), htitle.data(), zbin.size()-1, zbins.data(), 800, 0, 16000 );
		
		hname = "Ex_T1_cut" + std::to_string(j);
		htitle = "Excitation energy for user cut " + std::to_string(j);
		htitle += " with a time gate on T1 proton pulse;Excitation energy [keV];Counts per 20 keV";
		Ex_T1_cut[j] = new TH1F( hname.data(), htitle.data(), 850, -2000, 15000 );
		
		hname = "Ex_vs_T1_cut" + std::to_string(j);
		htitle = "Excitation energy as a function of time since T1 proton pulse;Event time - T1 [ns];Excitation energy [keV];Counts per 20 keV";
		Ex_vs_T1_cut[j] = new TH2F( hname.data(), htitle.data(), 1000, 0, 100e9, 800, -1000, 15000 );

		hname = "Ex_vs_theta_T1_cut" + std::to_string(j);
		htitle = "Excitation energy vs. centre of mass angle for user cut " + std::to_string(j);
		htitle += " with a time gate on T1 proton pulse;#theta_{CM} [deg];Excitation energy [keV];Counts per deg per 20 keV";
		Ex_vs_theta_T1_cut[j] = new TH2F( hname.data(), htitle.data(), 180, 0, 180.0, 850, -2000, 15000 );
		
		hname = "Ex_vs_z_T1_cut" + std::to_string(j);
		htitle = "Excitation energy vs. measured z for user cut " + std::to_string(j);
		htitle += " with a time gate on T1 proton pulse;z [mm];Excitation energy [keV];Counts per mm per 20 keV";
		Ex_vs_z_T1_cut[j] = new TH2F( hname.data(), htitle.data(), zbin.size()-1, zbins.data(), 850, -2000, 15000 );
		
	} // Array

	
	// For timing
	dirname = "Timing";
	output_file->mkdir( dirname.data() );
	
	// For recoil sectors
	dirname = "RecoilDetector";
	output_file->mkdir( dirname.data() );
	output_file->cd( dirname.data() );
	
	recoil_array_td.resize( set->GetNumberOfRecoilSectors() );
	recoil_elum_td.resize( set->GetNumberOfRecoilSectors() );
	recoil_EdE.resize( set->GetNumberOfRecoilSectors() );
	recoil_EdE_cut.resize( set->GetNumberOfRecoilSectors() );
	recoil_EdE_array.resize( set->GetNumberOfRecoilSectors() );
	recoil_bragg.resize( set->GetNumberOfRecoilSectors() );
	recoil_dE_vs_T1.resize( set->GetNumberOfRecoilSectors() );

	// Loop over each recoil sector
	for( unsigned int i = 0; i < set->GetNumberOfRecoilSectors(); ++i ) {
		
		dirname = "RecoilDetector/sector_" + std::to_string(i);
		output_file->mkdir( dirname.data() );
		output_file->cd( dirname.data() );
		
		// Recoil energy plots
		hname = "recoil_EdE_sec" + std::to_string(i);
		htitle = "Recoil dE-E plot for sector " + std::to_string(i);
		htitle += " - singles;Rest energy, E [keV];Energy loss, dE [keV];Counts";
		recoil_EdE[i] = new TH2F( hname.data(), htitle.data(),
								 2000, 0, 200000, 2000, 0, 200000 );
		
		hname = "recoil_EdE_cut_sec" + std::to_string(i);
		htitle = "Recoil dE-E plot for sector " + std::to_string(i);
		htitle += " - with energy cut;Rest energy, E [keV];Energy loss, dE [keV];Counts";
		recoil_EdE_cut[i] = new TH2F( hname.data(), htitle.data(),
									 2000, 0, 200000, 2000, 0, 200000 );
		
		hname = "recoil_EdE_array_sec" + std::to_string(i);
		htitle = "Recoil dE-E plot for sector " + std::to_string(i);
		htitle += " - in coincidence with array;Rest energy, E [keV];Energy loss, dE [keV];Counts";
		recoil_EdE_array[i] = new TH2F( hname.data(), htitle.data(),
									   2000, 0, 200000, 2000, 0, 200000 );
		
		hname = "recoil_bragg_sec" + std::to_string(i);
		htitle = "Recoil Bragg plot for sector " + std::to_string(i);
		htitle += ";Bragg ID;Energy loss, dE [keV];Counts";
		recoil_bragg[i] = new TH2F( hname.data(), htitle.data(),
								 set->GetNumberOfRecoilLayers(), -0.5, set->GetNumberOfRecoilLayers()-0.5, 2000, 0, 200000 );

		hname = "recoil_dE_vs_T1_sec" + std::to_string(i);
		htitle = "Recoil dE plot versus T1 time for sector " + std::to_string(i);
		htitle += ";Time since T1 proton pulse [ns];Energy loss, dE [keV];Counts";
		recoil_dE_vs_T1[i] = new TH2F( hname.data(), htitle.data(),
								 5000, 0, 50e9, 2000, 0, 200000 );

		// Timing plots
		output_file->cd( "Timing" );
		recoil_array_td[i].resize( set->GetNumberOfArrayModules() );
		recoil_elum_td[i].resize( set->GetNumberOfELUMSectors() );

		// For array modules
		for( unsigned int j = 0; j < set->GetNumberOfArrayModules(); ++j ) {
			
			hname = "td_recoil_array_sec" + std::to_string(i) + "_mod" + std::to_string(j);
			htitle = "Time difference between recoil sector " + std::to_string(i);
			htitle += " and array module " + std::to_string(j);
			htitle += ";#Deltat;Counts";
			recoil_array_td[i][j] = new TH1F( hname.data(), htitle.data(),
											 1000, -1.0*set->GetEventWindow()-50, 1.0*set->GetEventWindow()+50 );
			
		}
		
		// For ELUM sectors
		for( unsigned int j = 0; j < set->GetNumberOfELUMSectors(); ++j ) {
			
			hname = "td_recoil_elum_sec" + std::to_string(i) + "_mod" + std::to_string(j);
			htitle = "Time difference between recoil sector " + std::to_string(i);
			htitle += " and ELUM sector " + std::to_string(j);
			htitle += ";#Deltat;Counts";
			recoil_elum_td[i][j] = new TH1F( hname.data(), htitle.data(),
											1000, -1.0*set->GetEventWindow()-50, 1.0*set->GetEventWindow()+50 );
			
		}
		
	} // Recoils
	
	// Recoil-array time walk
	output_file->cd( "Timing" );
	recoil_array_tw = new TH2F( "tw_recoil_array",
							   "Time-walk histogram for array-recoil coincidences;#Deltat [ns];Array energy [keV];Counts",
							   1000, -1.0*set->GetEventWindow(), 1.0*set->GetEventWindow(),
							   800, 0, 16000 );
	recoil_array_tw_prof = new TProfile( "tw_recoil_array_prof", "Time-walk profile for recoil-array coincidences;Array energy;#Delta t", 2000, 0, 60000 );
	
	recoil_array_tw_row.resize( set->GetNumberOfArrayModules() );
	
	// Loop over ISS modules
	for( unsigned int i = 0; i < set->GetNumberOfArrayModules(); ++i ) {
		
		recoil_array_tw_row[i].resize( set->GetNumberOfArrayRows() );
		
		// Loop over rows of the array
		for( unsigned int j = 0; j < set->GetNumberOfArrayRows(); ++j ) {
			
			hname = "tw_recoil_array_mod_" + std::to_string(i) + "_row" + std::to_string(j);
			htitle = "Time-walk histogram for array-recoil coincidences (module ";
			htitle += std::to_string(i) + ", row " + std::to_string(j) + ");Deltat [ns];Array energy [keV];Counts";
			recoil_array_tw_row[i][j] = new TH2F( hname.data(), htitle.data(), 1000, -1.0*set->GetEventWindow(), 1.0*set->GetEventWindow(),
												 800, 0, 16000 );
			
		}
		
	}
	
	
	// EBIS time windows
	output_file->cd( "Timing" );
	ebis_td_recoil = new TH1F( "ebis_td_recoil", "Recoil time with respect to EBIS;#Deltat;Counts per 20 #mus", 5.5e3, -0.1e8, 1e8  );
	ebis_td_array = new TH1F( "ebis_td_array", "Array time with respect to EBIS;#Deltat;Counts per 20 #mus", 5.5e3, -0.1e8, 1e8  );
	ebis_td_elum = new TH1F( "ebis_td_elum", "ELUM time with respect to EBIS;#Deltat;Counts per 20 #mus", 5.5e3, -0.1e8, 1e8  );
	
	// Supercycle and proton pulses
	t1_td_recoil = new TH1F( "t1_td_recoil", "Recoil time difference with respect to the T1;#Deltat;Counts per 20 #mus", 5.5e3, -0.1e11, 1e11 );
	sc_td_recoil = new TH1F( "sc_td_recoil", "Recoil time difference with respect to the SuperCycle;#Deltat;Counts per 20 #mus", 5.5e3, -0.1e11, 1e11 );

	
	// For ELUM sectors
	dirname = "ElumDetector";
	output_file->mkdir( dirname.data() );
	output_file->cd( dirname.data() );
	
	elum = new TH1F( "elum", "ELUM singles;Energy (keV);Counts per 5 keV", 10000, 0, 50000 );
	elum_ebis = new TH1F( "elum_ebis", "ELUM gated by EBIS and off beam subtracted;Energy (keV);Counts per 5 keV", 10000, 0, 50000 );
	elum_ebis_on = new TH1F( "elum_ebis_on", "ELUM gated on EBIS;Energy (keV);Counts per 5 keV", 10000, 0, 50000 );
	elum_ebis_off = new TH1F( "elum_ebis_off", "ELUM gated off EBIS;Energy (keV);Counts per 5 keV", 10000, 0, 50000 );
	elum_recoil = new TH1F( "elum_recoil", "ELUM gate on recoils;Energy (keV);Counts per 5 keV", 10000, 0, 50000 );
	elum_recoilT = new TH1F( "elum_recoilT", "ELUM with time gate on all recoils;Energy (keV);Counts per 5 keV", 10000, 0, 50000 );
	elum_vs_T1 = new TH2F( "elum_vs_T1", "ELUM energy versus T1 time (gated on EBIS);Energy (keV);Counts per 5 keV", 5000, 0, 50e9, 10000, 0, 50000 );

	elum_sec.resize( set->GetNumberOfELUMSectors() );
	elum_ebis_sec.resize( set->GetNumberOfELUMSectors() );
	elum_ebis_on_sec.resize( set->GetNumberOfELUMSectors() );
	elum_ebis_off_sec.resize( set->GetNumberOfELUMSectors() );
	elum_recoil_sec.resize( set->GetNumberOfELUMSectors() );
	elum_recoilT_sec.resize( set->GetNumberOfELUMSectors() );

	for( unsigned int j = 0; j < set->GetNumberOfELUMSectors(); ++j ) {
		
		dirname = "ElumDetector/sector_" + std::to_string(j);
		output_file->mkdir( dirname.data() );
		output_file->cd( dirname.data() );
		
		hname = "elum_sec" + std::to_string(j);
		htitle = "ELUM singles for sector " + std::to_string(j);
		htitle += ";Energy [keV];Counts 5 keV";
		elum_sec[j] = new TH1F( hname.data(), htitle.data(), 10000, 0, 50000 );
		
		hname = "elum_ebis_sec" + std::to_string(j);
		htitle = "ELUM events for sector " + std::to_string(j);
		htitle += " gated by EBIS and off beam subtracted;Energy [keV];Counts 5 keV";
		elum_ebis_sec[j] = new TH1F( hname.data(), htitle.data(), 10000, 0, 50000 );
		
		hname = "elum_ebis_on_sec" + std::to_string(j);
		htitle = "ELUM events for sector " + std::to_string(j);
		htitle += " gated on EBIS;Energy [keV];Counts 5 keV";
		elum_ebis_on_sec[j] = new TH1F( hname.data(), htitle.data(), 10000, 0, 50000 );
		
		hname = "elum_ebis_off_sec" + std::to_string(j);
		htitle = "ELUM events for sector " + std::to_string(j);
		htitle += " gated off EBIS;Energy [keV];Counts 5 keV";
		elum_ebis_off_sec[j] = new TH1F( hname.data(), htitle.data(), 10000, 0, 50000 );
		
		hname = "elum_recoil_sec" + std::to_string(j);
		htitle = "ELUM singles for sector " + std::to_string(j);
		htitle += " gated on recoils;Energy [keV];Counts 5 keV";
		elum_recoil_sec[j] = new TH1F( hname.data(), htitle.data(), 10000, 0, 50000 );
		
		hname = "elum_recoilT_sec" + std::to_string(j);
		htitle = "ELUM singles for sector " + std::to_string(j);
		htitle += " with a time gate on all recoils;Energy [keV];Counts 5 keV";
		elum_recoilT_sec[j] = new TH1F( hname.data(), htitle.data(), 10000, 0, 50000 );
		
		
	} // ELUM
	
}


void ISSHistogrammer::ResetHists() {
	
	std::cout << "in ISSHistogrammer::Reset_Hist()" << std::endl;
	
	// Timing
	for( unsigned int i = 0; i < recoil_array_td.size(); ++i )
		for( unsigned int j = 0; j < recoil_array_td[i].size(); ++j )
			recoil_array_td[i][j]->Reset("ICESM");
	
	for( unsigned int i = 0; i < recoil_elum_td.size(); ++i )
		for( unsigned int j = 0; j < recoil_elum_td[i].size(); ++j )
			recoil_elum_td[i][j]->Reset("ICESM");
	
	for( unsigned int i = 0; i < recoil_array_tw_row.size(); ++i )
		for( unsigned int j = 0; j < recoil_array_tw_row[i].size(); ++j )
			recoil_array_tw_row[i][j]->Reset("ICESM");
	
	
	//recoil_array_tw_prof->Reset("ICESM");
	ebis_td_recoil->Reset("ICESM");
	ebis_td_array->Reset("ICESM");
	ebis_td_elum->Reset("ICESM");
	t1_td_recoil->Reset("ICESM");
	sc_td_recoil->Reset("ICESM");
	recoil_array_tw->Reset("ICESM");
	
	// Recoils
	for( unsigned int i = 0; i < recoil_EdE.size(); ++i )
		recoil_EdE[i]->Reset("ICESM");
	
	for( unsigned int i = 0; i < recoil_EdE_cut.size(); ++i )
		recoil_EdE_cut[i]->Reset("ICESM");
	
	for( unsigned int i = 0; i < recoil_bragg.size(); ++i )
		recoil_bragg[i]->Reset("ICESM");

	for( unsigned int i = 0; i < recoil_dE_vs_T1.size(); ++i )
		recoil_dE_vs_T1[i]->Reset("ICESM");


	// Array - E vs. z
	E_vs_z->Reset("ICESM");
	E_vs_z_ebis->Reset("ICESM");
	E_vs_z_ebis_on->Reset("ICESM");
	E_vs_z_ebis_off->Reset("ICESM");
	E_vs_z_recoil->Reset("ICESM");
	E_vs_z_recoilT->Reset("ICESM");
	E_vs_z_T1->Reset("ICESM");

	for( unsigned int i = 0; i < E_vs_z_recoil_cut.size(); ++i )
		E_vs_z_recoil_cut[i]->Reset("ICESM");
	
	for( unsigned int i = 0; i < E_vs_z_recoilT_cut.size(); ++i )
		E_vs_z_recoilT_cut[i]->Reset("ICESM");
	
	for( unsigned int i = 0; i < E_vs_z_T1_cut.size(); ++i )
		E_vs_z_T1_cut[i]->Reset("ICESM");
	
	for( unsigned int i = 0; i < E_vs_z_ebis_off_cut.size(); ++i )
		E_vs_z_ebis_off_cut[i]->Reset("ICESM");
	
	for( unsigned int i = 0; i < E_vs_z_ebis_on_cut.size(); ++i )
		E_vs_z_ebis_on_cut[i]->Reset("ICESM");
	
	for( unsigned int i = 0; i < E_vs_z_ebis_cut.size(); ++i )
		E_vs_z_ebis_cut[i]->Reset("ICESM");
	
	for( unsigned int i = 0; i < E_vs_z_mod.size(); ++i )
		E_vs_z_mod[i]->Reset("ICESM");
	
	for( unsigned int i = 0; i < E_vs_z_ebis_mod.size(); ++i )
		E_vs_z_ebis_mod[i]->Reset("ICESM");
	
	for( unsigned int i = 0; i < E_vs_z_ebis_on_mod.size(); ++i )
		E_vs_z_ebis_on_mod[i]->Reset("ICESM");
	
	for( unsigned int i = 0; i < E_vs_z_ebis_off_mod.size(); ++i )
		E_vs_z_ebis_off_mod[i]->Reset("ICESM");
	
	for( unsigned int i = 0; i < E_vs_z_recoil_mod.size(); ++i )
		E_vs_z_recoil_mod[i]->Reset("ICESM");
	
	for( unsigned int i = 0; i < E_vs_z_recoilT_mod.size(); ++i )
		E_vs_z_recoilT_mod[i]->Reset("ICESM");
	
	for( unsigned int i = 0; i < E_vs_z_cut.size(); ++i )
		E_vs_z_cut[i]->Reset("ICESM");
	
	// Array - Ex vs. thetaCM
	Ex_vs_theta->Reset("ICESM");
	Ex_vs_theta_ebis->Reset("ICESM");
	Ex_vs_theta_ebis_on->Reset("ICESM");
	Ex_vs_theta_ebis_off->Reset("ICESM");
	Ex_vs_theta_recoil->Reset("ICESM");
	Ex_vs_theta_recoilT->Reset("ICESM");
	Ex_vs_theta_T1->Reset("ICESM");

	for( unsigned int i = 0; i < Ex_vs_theta_mod.size(); ++i )
		Ex_vs_theta_mod[i]->Reset("ICESM");
	
	for( unsigned int i = 0; i < Ex_vs_theta_ebis_mod.size(); ++i )
		Ex_vs_theta_ebis_mod[i]->Reset("ICESM");
	
	for( unsigned int i = 0; i < Ex_vs_theta_recoilT_mod.size(); ++i )
		Ex_vs_theta_recoilT_mod[i]->Reset("ICESM");
	
	for( unsigned int i = 0; i < Ex_vs_theta_recoil_mod.size(); ++i )
		Ex_vs_theta_recoil_mod[i]->Reset("ICESM");
	
	for( unsigned int i = 0; i < Ex_vs_theta_ebis_off_mod.size(); ++i )
		Ex_vs_theta_ebis_off_mod[i]->Reset("ICESM");
	
	for( unsigned int i = 0; i < Ex_vs_theta_ebis_on_mod.size(); ++i )
		Ex_vs_theta_ebis_on_mod[i]->Reset("ICESM");
	
	for( unsigned int i = 0; i < Ex_vs_theta_ebis_on_cut.size(); ++i )
		Ex_vs_theta_ebis_on_cut[i]->Reset("ICESM");
	
	for( unsigned int i = 0; i < Ex_vs_theta_ebis_cut.size(); ++i )
		Ex_vs_theta_ebis_cut[i]->Reset("ICESM");
	
	for( unsigned int i = 0; i < Ex_vs_theta_cut.size(); ++i )
		Ex_vs_theta_cut[i]->Reset("ICESM");
	
	for( unsigned int i = 0; i < Ex_vs_theta_recoilT_cut.size(); ++i )
		Ex_vs_theta_recoilT_cut[i]->Reset("ICESM");
	
	for( unsigned int i = 0; i < Ex_vs_theta_T1_cut.size(); ++i )
		Ex_vs_theta_T1_cut[i]->Reset("ICESM");
	
	for( unsigned int i = 0; i < Ex_vs_theta_recoil_cut.size(); ++i )
		Ex_vs_theta_recoil_cut[i]->Reset("ICESM");
	
	for( unsigned int i = 0; i < Ex_vs_theta_ebis_off_cut.size(); ++i )
		Ex_vs_theta_ebis_off_cut[i]->Reset("ICESM");
	
	// Array - Ex vs. z
	Ex_vs_z->Reset("ICESM");
	Ex_vs_z_ebis->Reset("ICESM");
	Ex_vs_z_ebis_on->Reset("ICESM");
	Ex_vs_z_ebis_off->Reset("ICESM");
	Ex_vs_z_recoil->Reset("ICESM");
	Ex_vs_z_recoilT->Reset("ICESM");
	Ex_vs_z_T1->Reset("ICESM");

	for( unsigned int i = 0; i < Ex_vs_z_recoil_cut.size(); ++i )
		Ex_vs_z_recoil_cut[i]->Reset("ICESM");
	
	for( unsigned int i = 0; i < Ex_vs_z_recoilT_cut.size(); ++i )
		Ex_vs_z_recoilT_cut[i]->Reset("ICESM");

	for( unsigned int i = 0; i < Ex_vs_z_T1_cut.size(); ++i )
		Ex_vs_z_T1_cut[i]->Reset("ICESM");

	for( unsigned int i = 0; i < Ex_vs_z_mod.size(); ++i )
		Ex_vs_z_mod[i]->Reset("ICESM");
	
	for( unsigned int i = 0; i < Ex_vs_z_ebis_mod.size(); ++i )
		Ex_vs_z_ebis_mod[i]->Reset("ICESM");
	
	for( unsigned int i = 0; i < Ex_vs_z_ebis_on_mod.size(); ++i )
		Ex_vs_z_ebis_on_mod[i]->Reset("ICESM");
	
	for( unsigned int i = 0; i < Ex_vs_z_ebis_off_mod.size(); ++i )
		Ex_vs_z_ebis_off_mod[i]->Reset("ICESM");
	
	for( unsigned int i = 0; i < Ex_vs_z_recoil_mod.size(); ++i )
		Ex_vs_z_recoil_mod[i]->Reset("ICESM");
	
	for( unsigned int i = 0; i < Ex_vs_z_recoilT_mod.size(); ++i )
		Ex_vs_z_recoilT_mod[i]->Reset("ICESM");
	
	for( unsigned int i = 0; i < Ex_vs_z_cut.size(); ++i )
		Ex_vs_z_cut[i]->Reset("ICESM");
	
	for( unsigned int i = 0; i < Ex_vs_z_ebis_cut.size(); ++i )
		Ex_vs_z_ebis_cut[i]->Reset("ICESM");
	
	for( unsigned int i = 0; i < Ex_vs_z_ebis_on_cut.size(); ++i )
		Ex_vs_z_ebis_on_cut[i]->Reset("ICESM");
	
	for( unsigned int i = 0; i < Ex_vs_z_ebis_off_cut.size(); ++i )
		Ex_vs_z_ebis_off_cut[i]->Reset("ICESM");
	
	// Array - Ex
	Ex->Reset("ICESM");
	Ex_ebis->Reset("ICESM");
	Ex_ebis_on->Reset("ICESM");
	Ex_ebis_off->Reset("ICESM");
	Ex_recoil->Reset("ICESM");
	Ex_recoilT->Reset("ICESM");
	Ex_T1->Reset("ICESM");
	Ex_vs_T1->Reset("ICESM");

	for( unsigned int i = 0; i < Ex_recoil_cut.size(); ++i )
		Ex_recoil_cut[i]->Reset("ICESM");
	
	for( unsigned int i = 0; i < Ex_recoilT_cut.size(); ++i )
		Ex_recoilT_cut[i]->Reset("ICESM");
	
	for( unsigned int i = 0; i < Ex_T1_cut.size(); ++i )
		Ex_T1_cut[i]->Reset("ICESM");
	
	for( unsigned int i = 0; i < Ex_vs_T1_cut.size(); ++i )
		Ex_vs_T1_cut[i]->Reset("ICESM");
	
	for( unsigned int i = 0; i < Ex_mod.size(); ++i )
		Ex_mod[i]->Reset("ICESM");
	
	for( unsigned int i = 0; i < Ex_ebis_mod.size(); ++i )
		Ex_ebis_mod[i]->Reset("ICESM");
	
	for( unsigned int i = 0; i < Ex_ebis_on_mod.size(); ++i )
		Ex_ebis_on_mod[i]->Reset("ICESM");
	
	for( unsigned int i = 0; i < Ex_ebis_off_mod.size(); ++i )
		Ex_ebis_off_mod[i]->Reset("ICESM");
	
	for( unsigned int i = 0; i < Ex_recoil_mod.size(); ++i )
		Ex_recoil_mod[i]->Reset("ICESM");
	
	for( unsigned int i = 0; i < Ex_recoilT_mod.size(); ++i )
		Ex_recoilT_mod[i]->Reset("ICESM");
	
	for( unsigned int i = 0; i < Ex_cut.size(); ++i )
		Ex_cut[i]->Reset("ICESM");
	
	for( unsigned int i = 0; i < Ex_ebis_cut.size(); ++i )
		Ex_ebis_cut[i]->Reset("ICESM");
	
	for( unsigned int i = 0; i < Ex_ebis_on_cut.size(); ++i )
		Ex_ebis_on_cut[i]->Reset("ICESM");
	
	for( unsigned int i = 0; i < Ex_ebis_off_cut.size(); ++i )
		Ex_ebis_off_cut[i]->Reset("ICESM");
	
	
	
	// ELUM
	for( unsigned int i = 0; i < elum_sec.size(); ++i )
		elum_sec[i]->Reset("ICESM");
	
	for( unsigned int i = 0; i < elum_ebis_sec.size(); ++i )
		elum_ebis_sec[i]->Reset("ICESM");
	
	for( unsigned int i = 0; i < elum_ebis_on_sec.size(); ++i )
		elum_ebis_on_sec[i]->Reset("ICESM");
	
	for( unsigned int i = 0; i < elum_ebis_off_sec.size(); ++i )
		elum_ebis_off_sec[i]->Reset("ICESM");
	
	for( unsigned int i = 0; i < elum_recoil_sec.size(); ++i )
		elum_recoil_sec[i]->Reset("ICESM");
	
	for( unsigned int i = 0; i < elum_recoilT_sec.size(); ++i )
		elum_recoilT_sec[i]->Reset("ICESM");
	
	elum->Reset("ICESM");
	elum_ebis->Reset("ICESM");
	elum_ebis_on->Reset("ICESM");
	elum_ebis_off->Reset("ICESM");
	elum_recoil->Reset("ICESM");
	elum_recoilT->Reset("ICESM");
	elum_vs_T1->Reset("ICESM");

	return;
	
}

unsigned long ISSHistogrammer::FillHists() {
	
	/// Main function to fill the histograms
	n_entries = input_tree->GetEntries();
	
	std::cout << " ISSHistogrammer: number of entries in event tree = ";
	std::cout << n_entries << std::endl;
	
	if( !n_entries ){
		
		std::cout << " ISSHistogrammer: Nothing to do..." << std::endl;
		return n_entries;
		
	}
	else {
		
		std::cout << " ISSHistogrammer: Start filling histograms" << std::endl;
		
	}
	
	// ------------------------------------------------------------------------ //
	// Main loop over TTree to find events
	// ------------------------------------------------------------------------ //
	for( unsigned int i = 0; i < n_entries; ++i ){
		
		// Current event data
		input_tree->GetEntry(i);
		
		// tdiff variable
		double tdiff;
		
		// Loop over array events
		// if you want the p-side only events, use GetArrayPMultiplicity
		// if you want the "normal" mode using p/n-coincidences, use GetArrayMultiplicity
		for( unsigned int j = 0; j < read_evts->GetArrayMultiplicity(); ++j ){
			//for( unsigned int j = 0; j < read_evts->GetArrayPMultiplicity(); ++j ){
			
			// Get array event (uncomment the option you want)
			// GetArrayEvt is "normal" mode
			// GetArrayPEvt is p-side only events
			array_evt = read_evts->GetArrayEvt(j);
			//array_evt = read_evts->GetArrayPEvt(j);
			
			// Do the reaction
			react->MakeReaction( array_evt->GetPosition(), array_evt->GetEnergy() );
			
			// Singles
			E_vs_z->Fill( react->GetZmeasured(), array_evt->GetEnergy() );
			E_vs_z_mod[array_evt->GetModule()]->Fill( react->GetZmeasured(), array_evt->GetEnergy() );
			Ex->Fill( react->GetEx() );
			Ex_mod[array_evt->GetModule()]->Fill( react->GetEx() );
			Ex_vs_theta->Fill( react->GetThetaCM() * TMath::RadToDeg(), react->GetEx() );
			Ex_vs_theta_mod[array_evt->GetModule()]->Fill( react->GetThetaCM() * TMath::RadToDeg(), react->GetEx() );
			Ex_vs_z->Fill( react->GetZmeasured(), react->GetEx() );
			Ex_vs_z_mod[array_evt->GetModule()]->Fill( react->GetZmeasured(), react->GetEx() );
			
			// Check the E vs z cuts from the user
			for( unsigned int k = 0; k < react->GetNumberOfEvsZCuts(); ++k ){
				
				// Is inside the cut
				if( react->GetEvsZCut(k)->IsInside( react->GetZmeasured(), array_evt->GetEnergy() ) ){
					
					E_vs_z_cut[k]->Fill( react->GetZmeasured(), array_evt->GetEnergy() );
					Ex_cut[k]->Fill( react->GetEx() );
					Ex_vs_theta_cut[k]->Fill( react->GetThetaCM() * TMath::RadToDeg(), react->GetEx() );
					Ex_vs_z_cut[k]->Fill( react->GetZmeasured(), react->GetEx() );
					
				} // inside cut
				
			} // loop over cuts
			
			
			// EBIS time
			ebis_td_array->Fill( (double)array_evt->GetTime() - (double)read_evts->GetEBIS() );
			
			// Check for events in the EBIS on-beam window
			if( OnBeam( array_evt ) ){
				
				E_vs_z_ebis->Fill( react->GetZmeasured(), array_evt->GetEnergy() );
				E_vs_z_ebis_on->Fill( react->GetZmeasured(), array_evt->GetEnergy() );
				E_vs_z_ebis_mod[array_evt->GetModule()]->Fill( react->GetZmeasured(), array_evt->GetEnergy() );
				E_vs_z_ebis_on_mod[array_evt->GetModule()]->Fill( react->GetZmeasured(), array_evt->GetEnergy() );
				Ex_ebis->Fill( react->GetEx() );
				Ex_ebis_on->Fill( react->GetEx() );
				Ex_ebis_mod[array_evt->GetModule()]->Fill( react->GetEx() );
				Ex_ebis_on_mod[array_evt->GetModule()]->Fill( react->GetEx() );
				Ex_vs_theta_ebis->Fill( react->GetThetaCM() * TMath::RadToDeg(), react->GetEx() );
				Ex_vs_theta_ebis_on->Fill( react->GetThetaCM() * TMath::RadToDeg(), react->GetEx() );
				Ex_vs_theta_ebis_mod[array_evt->GetModule()]->Fill( react->GetThetaCM() * TMath::RadToDeg(), react->GetEx() );
				Ex_vs_theta_ebis_on_mod[array_evt->GetModule()]->Fill( react->GetThetaCM() * TMath::RadToDeg(), react->GetEx() );
				Ex_vs_z_ebis->Fill( react->GetZmeasured(), react->GetEx() );
				Ex_vs_z_ebis_on->Fill( react->GetZmeasured(), react->GetEx() );
				Ex_vs_z_ebis_mod[array_evt->GetModule()]->Fill( react->GetZmeasured(), react->GetEx() );
				Ex_vs_z_ebis_on_mod[array_evt->GetModule()]->Fill( react->GetZmeasured(), react->GetEx() );
				
				// Check for events in the user-defined T1 window
				Ex_vs_T1->Fill( (double)array_evt->GetTime() - read_evts->GetT1(), react->GetEx() );
				if( T1Cut( array_evt ) ) {
					
					E_vs_z_T1->Fill( react->GetZmeasured(), array_evt->GetEnergy() );
					Ex_T1->Fill( react->GetEx() );
					Ex_vs_theta_T1->Fill( react->GetThetaCM() * TMath::RadToDeg(), react->GetEx() );
					Ex_vs_z_T1->Fill( react->GetZmeasured(), react->GetEx() );
				
				} // T1

				// Check the E vs z cuts from the user
				for( unsigned int k = 0; k < react->GetNumberOfEvsZCuts(); ++k ){
					
					// Is inside the cut
					if( react->GetEvsZCut(k)->IsInside( react->GetZmeasured(), array_evt->GetEnergy() ) ){
						
						E_vs_z_ebis_cut[k]->Fill( react->GetZmeasured(), array_evt->GetEnergy() );
						E_vs_z_ebis_on_cut[k]->Fill( react->GetZmeasured(), array_evt->GetEnergy() );
						Ex_ebis_cut[k]->Fill( react->GetEx() );
						Ex_ebis_on_cut[k]->Fill( react->GetEx() );
						Ex_vs_theta_ebis_cut[k]->Fill( react->GetThetaCM() * TMath::RadToDeg(), react->GetEx() );
						Ex_vs_theta_ebis_on_cut[k]->Fill( react->GetThetaCM() * TMath::RadToDeg(), react->GetEx() );
						Ex_vs_z_ebis_cut[k]->Fill( react->GetZmeasured(), react->GetEx() );
						Ex_vs_z_ebis_on_cut[k]->Fill( react->GetZmeasured(), react->GetEx() );
						
						// Check for events in the user-defined T1 window
						Ex_vs_T1_cut[k]->Fill( (double)array_evt->GetTime() - read_evts->GetT1(), react->GetEx() );
						if( T1Cut( array_evt ) ) {
							
							E_vs_z_T1_cut[k]->Fill( react->GetZmeasured(), array_evt->GetEnergy() );
							Ex_T1_cut[k]->Fill( react->GetEx() );
							Ex_vs_theta_T1_cut[k]->Fill( react->GetThetaCM() * TMath::RadToDeg(), react->GetEx() );
							Ex_vs_z_T1_cut[k]->Fill( react->GetZmeasured(), react->GetEx() );
						
						} // T1

					} // inside cut
					
				} // loop over cuts
				
			} // ebis
			
			else if( OffBeam( array_evt ) ){
				
				E_vs_z_ebis->Fill( react->GetZmeasured(), array_evt->GetEnergy(), -1.0 * react->GetEBISFillRatio() );
				E_vs_z_ebis_off->Fill( react->GetZmeasured(), array_evt->GetEnergy() );
				E_vs_z_ebis_mod[array_evt->GetModule()]->Fill( react->GetZmeasured(), array_evt->GetEnergy(), -1.0 * react->GetEBISFillRatio() );
				E_vs_z_ebis_off_mod[array_evt->GetModule()]->Fill( react->GetZmeasured(), array_evt->GetEnergy() );
				Ex_ebis->Fill( react->GetEx(), -1.0 * react->GetEBISFillRatio() );
				Ex_ebis_off->Fill( react->GetEx() );
				Ex_ebis_mod[array_evt->GetModule()]->Fill( react->GetEx(), -1.0 * react->GetEBISFillRatio() );
				Ex_ebis_off_mod[array_evt->GetModule()]->Fill( react->GetEx() );
				Ex_vs_theta_ebis->Fill( react->GetThetaCM() * TMath::RadToDeg(), react->GetEx(), -1.0 * react->GetEBISFillRatio() );
				Ex_vs_theta_ebis_mod[array_evt->GetModule()]->Fill( react->GetThetaCM() * TMath::RadToDeg(), react->GetEx() );
				Ex_vs_theta_ebis_off->Fill( react->GetThetaCM() * TMath::RadToDeg(), react->GetEx() );
				Ex_vs_theta_ebis_off_mod[array_evt->GetModule()]->Fill( react->GetThetaCM() * TMath::RadToDeg(), react->GetEx() );
				Ex_vs_z_ebis->Fill( react->GetZmeasured(), react->GetEx(), -1.0 * react->GetEBISFillRatio() );
				Ex_vs_z_ebis_mod[array_evt->GetModule()]->Fill( react->GetZmeasured(), react->GetEx(), -1.0 * react->GetEBISFillRatio() );
				Ex_vs_z_ebis_off->Fill( react->GetZmeasured(), react->GetEx() );
				Ex_vs_z_ebis_off_mod[array_evt->GetModule()]->Fill( react->GetZmeasured(), react->GetEx() );
				
				// Check the E vs z cuts from the user
				for( unsigned int k = 0; k < react->GetNumberOfEvsZCuts(); ++k ){
					
					// Is inside the cut
					if( react->GetEvsZCut(k)->IsInside( react->GetZmeasured(), array_evt->GetEnergy() ) ){
						
						E_vs_z_ebis_cut[k]->Fill( react->GetZmeasured(), array_evt->GetEnergy(), -1.0 * react->GetEBISFillRatio() );
						E_vs_z_ebis_off_cut[k]->Fill( react->GetZmeasured(), array_evt->GetEnergy() );
						Ex_ebis_cut[k]->Fill( react->GetEx(), -1.0 * react->GetEBISFillRatio() );
						Ex_ebis_off_cut[k]->Fill( react->GetEx() );
						Ex_vs_theta_ebis_cut[k]->Fill( react->GetThetaCM() * TMath::RadToDeg(), react->GetEx(), -1.0 * react->GetEBISFillRatio() );
						Ex_vs_theta_ebis_off_cut[k]->Fill( react->GetThetaCM() * TMath::RadToDeg(), react->GetEx() );
						Ex_vs_z_ebis_cut[k]->Fill( react->GetZmeasured(), react->GetEx(), -1.0 * react->GetEBISFillRatio() );
						Ex_vs_z_ebis_off_cut[k]->Fill( react->GetZmeasured(), react->GetEx() );
						
					} // inside cut
					
				} // loop over cuts
				
			} // off ebis
			
			// Loop over recoil events
			double tdiff_min = 99999.;
			int recoil_idx = -1;
			for( unsigned int k = 0; k < read_evts->GetRecoilMultiplicity(); ++k ){
				
				// Get recoil event
				recoil_evt = read_evts->GetRecoilEvt(k);
				
				// Time differences
				tdiff = (double)recoil_evt->GetTime() - (double)array_evt->GetTime();
				recoil_array_td[recoil_evt->GetSector()][array_evt->GetModule()]->Fill( tdiff );
				recoil_array_tw->Fill( tdiff, array_evt->GetEnergy() );
				recoil_array_tw_prof->Fill( array_evt->GetEnergy(), tdiff );
				
				for( unsigned int i = 0; i < set->GetNumberOfArrayModules(); ++i )
					for( unsigned int j = 0; j < set->GetNumberOfArrayRows(); ++j )
						if ( array_evt->GetModule() == i && array_evt->GetRow() == j )
							recoil_array_tw_row[i][j]->Fill( tdiff, array_evt->GetEnergy() );
				
				
				// Check which is recoil closest in time
				if( tdiff < tdiff_min ) {
					
					recoil_idx = k;
					tdiff_min = tdiff;
					
				}
				
			} // k
			
			// Only use the recoil closest in time
			// TODO: Improve this selection criteria
			if( recoil_idx >= 0 ) {
				
				// Get recoil event
				recoil_evt = read_evts->GetRecoilEvt( recoil_idx );

				// Check for prompt events with recoils
				if( PromptCoincidence( recoil_evt, array_evt ) ){
					
					// Recoils in coincidence with an array event
					recoil_EdE_array[recoil_evt->GetSector()]->Fill( recoil_evt->GetEnergyRest( set->GetRecoilEnergyRestStart(), set->GetRecoilEnergyRestStop() ), recoil_evt->GetEnergyLoss( set->GetRecoilEnergyLossStart(), set->GetRecoilEnergyLossStop() ) );
					
					// Array histograms
					E_vs_z_recoilT->Fill( react->GetZmeasured(), array_evt->GetEnergy() );
					E_vs_z_recoilT_mod[array_evt->GetModule()]->Fill( react->GetZmeasured(), array_evt->GetEnergy() );
					Ex_recoilT->Fill( react->GetEx() );
					Ex_recoilT_mod[array_evt->GetModule()]->Fill( react->GetEx() );
					Ex_vs_theta_recoilT->Fill( react->GetThetaCM() * TMath::RadToDeg(), react->GetEx() );
					Ex_vs_theta_recoilT_mod[array_evt->GetModule()]->Fill( react->GetThetaCM() * TMath::RadToDeg(), react->GetEx() );
					Ex_vs_z_recoilT->Fill( react->GetZmeasured(), react->GetEx() );
					Ex_vs_z_recoilT_mod[array_evt->GetModule()]->Fill( react->GetZmeasured(), react->GetEx() );
					
					// Check the E vs z cuts from the user
					for( unsigned int l = 0; l < react->GetNumberOfEvsZCuts(); ++l ){
						
						// Is inside the cut
						if( react->GetEvsZCut(l)->IsInside( react->GetZmeasured(), array_evt->GetEnergy() ) ){
							
							E_vs_z_recoilT_cut[l]->Fill( react->GetZmeasured(), array_evt->GetEnergy() );
							Ex_recoilT_cut[l]->Fill( react->GetEx() );
							Ex_vs_theta_recoilT_cut[l]->Fill( react->GetThetaCM() * TMath::RadToDeg(), react->GetEx() );
							Ex_vs_z_recoilT_cut[l]->Fill( react->GetZmeasured(), react->GetEx() );
							
						} // inside cut
						
					} // loop over cuts
					
					// Add an energy gate
					if( RecoilCut( recoil_evt ) ) {
						
						E_vs_z_recoil->Fill( react->GetZmeasured(), array_evt->GetEnergy() );
						E_vs_z_recoil_mod[array_evt->GetModule()]->Fill( react->GetZmeasured(), array_evt->GetEnergy() );
						Ex_recoil->Fill( react->GetEx() );
						Ex_recoil_mod[array_evt->GetModule()]->Fill( react->GetEx() );
						Ex_vs_theta_recoil->Fill( react->GetThetaCM() * TMath::RadToDeg(), react->GetEx() );
						Ex_vs_theta_recoil_mod[array_evt->GetModule()]->Fill( react->GetThetaCM() * TMath::RadToDeg(), react->GetEx() );
						Ex_vs_z_recoil->Fill( react->GetZmeasured(), react->GetEx() );
						Ex_vs_z_recoil_mod[array_evt->GetModule()]->Fill( react->GetZmeasured(), react->GetEx() );
						
						// Check the E vs z cuts from the user
						for( unsigned int l = 0; l < react->GetNumberOfEvsZCuts(); ++l ){
							
							// Is inside the cut
							if( react->GetEvsZCut(l)->IsInside( react->GetZmeasured(), array_evt->GetEnergy() ) ){
								
								E_vs_z_recoil_cut[l]->Fill( react->GetZmeasured(), array_evt->GetEnergy() );
								Ex_recoil_cut[l]->Fill( react->GetEx() );
								Ex_vs_theta_recoil_cut[l]->Fill( react->GetThetaCM() * TMath::RadToDeg(), react->GetEx() );
								Ex_vs_z_recoil_cut[l]->Fill( react->GetZmeasured(), react->GetEx() );
								
							} // inside cut
							
						} // loop over cuts
						
					} // energy cuts
					
				} // prompt
				
			} // just one recoil of interest
			
		} // array
		
		
		// Loop over ELUM events
		for( unsigned int j = 0; j < read_evts->GetElumMultiplicity(); ++j ){
			
			// Get ELUM event
			elum_evt = read_evts->GetElumEvt(j);
			
			// EBIS time
			ebis_td_elum->Fill( (double)elum_evt->GetTime() - (double)read_evts->GetEBIS() );
			
			// Singles
			elum->Fill( elum_evt->GetEnergy() );
			elum_sec[elum_evt->GetSector()]->Fill( elum_evt->GetEnergy() );
			
			// Check for events in the EBIS on-beam window
			if( OnBeam( elum_evt ) ){
				
				elum_ebis->Fill( elum_evt->GetEnergy() );
				elum_ebis_sec[elum_evt->GetSector()]->Fill( elum_evt->GetEnergy() );
				elum_ebis_on->Fill( elum_evt->GetEnergy() );
				elum_ebis_on_sec[elum_evt->GetSector()]->Fill( elum_evt->GetEnergy() );
				elum_vs_T1->Fill( (double)elum_evt->GetTime() - (double)read_evts->GetT1(), elum_evt->GetEnergy() );

			} // ebis
			
			else {
				
				elum_ebis->Fill( elum_evt->GetEnergy(), -1.0 * react->GetEBISFillRatio() );
				elum_ebis_sec[elum_evt->GetSector()]->Fill( elum_evt->GetEnergy(), -1.0 * react->GetEBISFillRatio() );
				elum_ebis_off->Fill( elum_evt->GetEnergy() );
				elum_ebis_off_sec[elum_evt->GetSector()]->Fill( elum_evt->GetEnergy() );
				
				
			}
			
			// Loop over recoil events
			for( unsigned int k = 0; k < read_evts->GetRecoilMultiplicity(); ++k ){
				
				// Get recoil event
				recoil_evt = read_evts->GetRecoilEvt(k);
				
				// Time differences
				tdiff = (double)recoil_evt->GetTime() - (double)elum_evt->GetTime();
				recoil_elum_td[recoil_evt->GetSector()][elum_evt->GetSector()]->Fill( tdiff );
				
				// Check for prompt events with recoils
				if( PromptCoincidence( recoil_evt, elum_evt ) ){
					
					elum_recoilT->Fill( elum_evt->GetEnergy() );
					elum_recoilT_sec[elum_evt->GetSector()]->Fill( elum_evt->GetEnergy() );
					
					// Add an energy gate
					if( RecoilCut( recoil_evt ) ) {
						
						elum_recoil->Fill( elum_evt->GetEnergy() );
						elum_recoil_sec[elum_evt->GetSector()]->Fill( elum_evt->GetEnergy() );
						
					} // energy cuts
					
				} // prompt
				
			} // recoils
			
		} // ELUM
		
		
		// Loop over recoil events
		for( unsigned int j = 0; j < read_evts->GetRecoilMultiplicity(); ++j ){
			
			// Get recoil event
			recoil_evt = read_evts->GetRecoilEvt(j);
			
			// EBIS, T1, SC time
			ebis_td_recoil->Fill( (double)recoil_evt->GetTime() - (double)read_evts->GetEBIS() );
			t1_td_recoil->Fill( (double)recoil_evt->GetTime() - (double)read_evts->GetT1() );
			sc_td_recoil->Fill( (double)recoil_evt->GetTime() - (double)read_evts->GetSC() );

			// Energy EdE plot, unconditioned
			recoil_EdE[recoil_evt->GetSector()]->Fill( recoil_evt->GetEnergyRest( set->GetRecoilEnergyRestStart(), set->GetRecoilEnergyRestStop() ),
													  recoil_evt->GetEnergyLoss( set->GetRecoilEnergyLossStart(), set->GetRecoilEnergyLossStop() ) );

			// Energy dE versus T1 time
			recoil_dE_vs_T1[recoil_evt->GetSector()]->Fill( (double)recoil_evt->GetTime() - (double)read_evts->GetT1(),
														   recoil_evt->GetEnergyLoss( set->GetRecoilEnergyLossStart(), set->GetRecoilEnergyLossStop() ) );

			// Bragg curve
			for( unsigned int k = 0; k < recoil_evt->GetEnergies().size(); ++k )
				recoil_bragg[recoil_evt->GetSector()]->Fill( recoil_evt->GetID(k), recoil_evt->GetEnergy(k) );
			
			// Energy EdE plot, after cut
			if( RecoilCut( recoil_evt ) )
				recoil_EdE_cut[recoil_evt->GetSector()]->Fill( recoil_evt->GetEnergyRest( set->GetRecoilEnergyRestStart(), set->GetRecoilEnergyRestStop() ),
															  recoil_evt->GetEnergyLoss( set->GetRecoilEnergyLossStart(), set->GetRecoilEnergyLossStop() ) );
			
		} // recoils
		
		// Progress bar
		bool update_progress = false;
		if( n_entries < 200 )
			update_progress = true;
		else if( i % (n_entries/100) == 0 || i+1 == n_entries )
			update_progress = true;
		
		if( update_progress ) {
			
			// Percent complete
			float percent = (float)(i+1)*100.0/(float)n_entries;
			
			// Progress bar in GUI
			if( _prog_ ) {
				
				prog->SetPosition( percent );
				gSystem->ProcessEvents();
				
			}
			
			// Progress bar in terminal
			std::cout << " " << std::setw(6) << std::setprecision(4);
			std::cout << percent << "%    \r";
			std::cout.flush();
			
		}
		
		
	} // all events
	
	output_file->Write();
	
	return n_entries;
	
}

void ISSHistogrammer::SetInputFile( std::vector<std::string> input_file_names ) {
	
	/// Overlaaded function for a single file or multiple files
	input_tree = new TChain( "evt_tree" );
	for( unsigned int i = 0; i < input_file_names.size(); i++ ) {
		
		input_tree->Add( input_file_names[i].data() );
		
	}
	input_tree->SetBranchAddress( "ISSEvts", &read_evts );
	
	return;
	
}

void ISSHistogrammer::SetInputFile( std::string input_file_name ) {
	
	/// Overloaded function for a single file or multiple files
	input_tree = new TChain( "evt_tree" );
	input_tree->Add( input_file_name.data() );
	input_tree->SetBranchAddress( "ISSEvts", &read_evts );
	
	return;
	
}

void ISSHistogrammer::SetInputTree( TTree *user_tree ){
	
	// Find the tree and set branch addresses
	input_tree = (TChain*)user_tree;
	input_tree->SetBranchAddress( "ISSEvts", &read_evts );
	
	return;
	
}
