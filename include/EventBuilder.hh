#ifndef _EventBuilder_hh
#define _EventBuilder_hh

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <memory>

#include <TFile.h>
#include <TTree.h>
#include <TMath.h>
#include <TChain.h>
#include <TH1.h>
#include <TH2.h>
#include <TProfile.h>
#include <TVector2.h>
#include <TVector3.h>
#include <TGProgressBar.h>
#include <TSystem.h>


// Settings header
#ifndef __SETTINGS_HH
# include "Settings.hh"
#endif

// Calibration header
#ifndef __CALIBRATION_HH
# include "Calibration.hh"
#endif

// Data packets header
#ifndef __DATAPACKETS_hh
# include "DataPackets.hh"
#endif

// ISS Events tree
#ifndef __ISSEVTS_HH
# include "ISSEvts.hh"
#endif

// Reaction header
#ifndef __REACTION_HH
# include "Reaction.hh"
#endif

// Histogram header
#ifndef __HISTOGRAMMER_hh
# include "Histogrammer.hh"
#endif

/*!
* \brief Builds physics events after all hits have been time sorted.
*
* \details The ISSEventBuilder Class takes a list of time-sorted events from all of the detectors, and packages them up into a series of physics events. The time-sorted events are looped over, and each different data type (ASIC/CAEN/INFO) is dealt with appropriately, and stored as their respective different data types. Any ASIC or CAEN hit above threshold is able to open an event window (specified by the user in the settings file which goes with the ISSSettings class). The next entry in the tree is probed in order to work out whether the event window should be closed. 
*
* When the event window closes, each detector has its own "finder function": ISSEventBuilder::ArrayFinder, ISSEventBuilder::RecoilFinder, ISSEventBuilder::MwpcFinder, ISSEventBuilder::ElumFinder, and ISSEventBuilder::ZeroDegreeFinder. These functions process the events on each detector, imposing prompt coincidence conditions amongst other sanity checks. Once processed, all of these hits on the different detectors are packaged up into a single event in an ISSEvts tree.
* The constructor for this class requires an ISSSettings object, which allows it to use parameters defined in the "settings.dat" file. This includes:
* - Settings which encode the wiring of the detectors e.g. the number of CAEN modules used.
* - The size of the event window used to combine events
*
* This size of the event window is crucial for determining which signals belong to which events. The default parameter for this is 3 microseconds *which is currently a blind choice that seems to work*. Perhaps you, dear reader, can come up with a more rigorous reason!
*/

class ISSEventBuilder {
	
public:
	
	ISSEventBuilder( ISSSettings *myset ); ///< Constructor
	virtual ~ISSEventBuilder(){}; /// Destructor (currently empty)

	void	SetInputFile( std::string input_file_name ); ///< Function to set the input file from which events are built
	void	SetInputTree( TTree* user_tree ); ///< Grabs the input tree from the input file defined in ISSEventBuilder::SetInputFile
	void	SetOutput( std::string output_file_name ); ///< Configures the output for the class
	void	StartFile();	///< Called for every file
	void	Initialise();	///< Called for every event
	void	MakeHists(); ///< Creates histograms for events that occur
	void	ResetHists(); ///< Empties the histograms during the DataSpy

	/// Adds the calibration from the external calibration file to the class
	/// \param[in] mycal The ISSCalibration object which is constructed by the ISSCalibration constructor used in iss_sort.cc
	inline void AddCalibration( ISSCalibration *mycal ){
		cal = mycal;
		overwrite_cal = true;
	};
	
	unsigned long	BuildEvents(); ///< The heart of this class

	// Resolve multiplicities etc
	void ArrayFinder(); ///< Processes all hits on the array that fall within the build window
	void RecoilFinder(); ///< Processes all hits on the recoil detector that fall within the build window
	void MwpcFinder(); ///< Processes all hits on the MWPC that fall within the build window
	void ElumFinder(); ///< Processes all hits on the ELUM that fall within the build window
	void ZeroDegreeFinder(); ///< Processes all hits on the zero-degree detector that fall within the build window
	void GammaRayFinder(); ///< Processes hits in the ScintArray and maybe HPGe in the future
		
	inline TFile* GetFile(){ return output_file; }; ///< Getter for the output_file pointer
	inline TTree* GetTree(){ return output_tree; }; ///< Getter for the output tree pointer
	
	inline void CloseOutput(){
		output_tree->ResetBranchAddresses();
		output_file->Close();
		input_tree->ResetBranchAddresses();
		input_file->Close();
		delete in_data;
		log_file.close(); //?? to close or not to close?
	}; ///< Closes the output files from this class
	void CleanHists(); ///< Deletes histograms from memory and clears vectors that store histograms

	inline void AddProgressBar( std::shared_ptr<TGProgressBar> myprog ){
		prog = myprog;
		_prog_ = true;
	}; ///< Adds a progress bar to the GUI
	///< \param[in] myprog pointer to the EventBuilder progress bar for the GUI


private:
	
	/// Input treze
	TFile *input_file; ///< Pointer to the time-sorted input ROOT file
	TTree *input_tree; ///< Pointer to the TTree in the input file
	ISSDataPackets *in_data; ///< Pointer to the TBranch containing the data in the time-sorted input ROOT file
	std::shared_ptr<ISSAsicData> asic_data; ///< Pointer to a given entry in the tree of some data from the ASICs
	std::shared_ptr<ISSCaenData> caen_data; ///< Pointer to a given entry in the tree of some data from the CAEN
	std::shared_ptr<ISSInfoData> info_data; ///< Pointer to a given entry in the tree of the "info" datatype

	/// Event structures
	std::shared_ptr<ISSArrayEvt> array_evt;
	std::shared_ptr<ISSArrayPEvt> arrayp_evt;
	std::shared_ptr<ISSRecoilEvt> recoil_evt;
	std::shared_ptr<ISSMwpcEvt> mwpc_evt;
	std::shared_ptr<ISSElumEvt> elum_evt;
	std::shared_ptr<ISSZeroDegreeEvt> zd_evt;
	std::shared_ptr<ISSGammaRayEvt> gamma_evt;

	/// Outputs
	TFile *output_file; ///< Pointer to the output ROOT file containing events
	TTree *output_tree; ///< Pointer to the output ROOT tree containing events
	std::unique_ptr<ISSEvts> write_evts; ///< Container for storing hits on all detectors in order to construct events
	
	// Do calibration
	ISSCalibration *cal; ///< Pointer to an ISSCalibration object, used for accessing gain-matching parameters and thresholds
	bool overwrite_cal; ///< Boolean determining whether an energy calibration should be used (true) or not (false). Set in the ISSEventBuilder::AddCalibration function
	
	// Settings file
	ISSSettings *set; ///< Pointer to the settings object. Assigned in constructor
	
	// Progress bar
	bool _prog_; ///< Boolean determining if there is a progress bar (in the GUI)
	std::shared_ptr<TGProgressBar> prog; ///< Progress bar for the GUI

	// Log file
	std::ofstream log_file; ///< Log file for recording the results of the ISSEventBuilder
	
	// Flag to know we've opened a file on disk
	bool flag_input_file;

	// These things are in the settings file
	long build_window;  ///< Length of build window in ns
	
	// Some more things that should be in a settings file
	std::vector<unsigned char> asic_side; ///< Vector containing 0 for p-side and 1 for n-side where the index is the asic number
	std::vector<unsigned char> asic_row; ///< Vector containing the smallest row number for a given p/n-side asic where the index is the asic number
	std::vector<std::vector<unsigned char>> array_row; ///< Gives the row of the array for each channel (accessed via asic number and channel number on strip). Unused channels have their value as 0
	std::vector<std::vector<int>> array_pid; ///< Gives each p-side strip on the array a unique number for identification (accessed via asic number and channel number on strip)
	std::vector<std::vector<int>> array_nid; ///< Gives each n-side strip on the array a number for identification (accessed via asic number and channel number on strip)

	// Flags
	bool flag_close_event; ///< Determines if the event should be closed for a given hit
	bool flag_caen_pulser; ///< Boolean for updating timing signals if the info data is from the caen pulser
	std::vector<bool> flag_pause;	///< Flags whether a pause signal has been sent for a gicen module on the array
	std::vector<bool> flag_resume;	//< Flags whether a resume signal has been sent for a gicen module on the array
	bool event_open; ///< Flag for deciding whether an event is currently being recorded or not

	// Time variables
	long		 		time_diff;	///< Time difference between first hit in event and current hit
	unsigned long long	time_prev;	///< Holds time of previous event
	unsigned long long	time_min;	///< The minimum time in an event containing hits
	unsigned long long	time_max;	///< The maximum time in an event containing hits
	unsigned long long	time_first;	///< Time of the first caen/asic/info event in a file
	unsigned long long  ebis_prev;	///< Holds time of previous ebis pulse
	unsigned long long  t1_prev;	///< Holds time of previous T1 pulse
	unsigned long long  sc_prev;	///< Holds time of previous SuperCycle pulse
	unsigned long long  laser_prev;	///< Holds time of previous Laser status pulse
	unsigned long long	caen_time;	///< Time from the caen DAQ
	unsigned long long	caen_prev;	///< Holds previous time from the CAEN DAQ
	std::vector<unsigned long long> fpga_time; 			///< FPGA time on a given module of the array
	std::vector<unsigned long long> fpga_prev;			///< Previous FPGA time on a given module of the array
	std::vector<unsigned long long> asic_time;			///< ASIC time on a given module of the array
	std::vector<unsigned long long> asic_prev;			///< Previous ASIC time on a given module of the array
	std::vector<unsigned long long> pause_time;			///< The pause time on a given module of the array
	std::vector<unsigned long long> resume_time;		///< The resume time on a given module of the array
	std::vector<unsigned long long> asic_dead_time;		///< ASIC dead time for a given module of the array
	std::vector<unsigned long long> asic_time_start;	///< Holds the time of the first hit on each asic in the input time-sorted tree (index denotes asic module)
	std::vector<unsigned long long> asic_time_stop;		///< Holds the time of the last hit on each asic in the input time-sorted tree (index denotes asic module)
	std::vector<unsigned long long> caen_time_start;	///< Holds the time of the first hit on each caen in the input time-sorted tree (index denotes caen module)
	std::vector<unsigned long long> caen_time_stop;		///< Holds the time of the last hit on each asic in the input time-sorted tree (index denotes caen module)

	// Data variables - generic
	unsigned char		mymod;		///< module number
	unsigned char		mych;		///< channel number
	unsigned long long	mytime;		///< absolute timestamp
	float 				myenergy;	///< calibrated energy
	int 				mywalk;		///< time walk correction
	bool				mythres;	///< above threshold?

	// Data variables - Array
	unsigned char		myasic;		///< ASIC number
	unsigned char		myside;		///< p-side = 0; n-side = 1
	unsigned char		myrow;		///< 4 wafers along array, 2 dE-E, 13 for gas
	int					mystrip;	///< strip number for DSSSD
	
	// Data variables - Recoil / ELUM / ZeroDegree / MWPC
	unsigned char		myid;		///< generic detector id
	unsigned char		mysector;	///< 4 quadrants of the recoil, for example
	unsigned char		mylayer;	///< 2 layers for the dE-E, for example


	// Array variables
	std::vector<float>		pen_list;	///< list of p-side energies for ParticleFinder
	std::vector<float>		nen_list;	///< list of n-side energies for ParticleFinder
	std::vector<long>		ptd_list;	///< list of p-side time differences for ParticleFinder
	std::vector<long>		ntd_list;	///< list of n-side time differences for ParticleFinder
	std::vector<char>		pid_list;	///< list of p-side strip ids
	std::vector<char>		nid_list;	///< list of n-side strip ids
	std::vector<char>		pmod_list;	///< list of p-side modules numbers
	std::vector<char>		nmod_list;	///< list of n-side modules numbers
	std::vector<char>		prow_list;	///< list of p-side row numbers
	std::vector<char>		nrow_list;	///< list of n-side row numbers

	// Recoil variables
	std::vector<float>	ren_list;	///< list of recoil energies for RecoilFinder
	std::vector<long>	rtd_list;	///< list of recoil time differences for RecoilFinder
	std::vector<char>	rid_list;	///< list of recoil IDs/layers for RecoilFinder
	std::vector<char>	rsec_list;	///< list of recoil sectors for RecoilFinder
	
	// MWPC variables
	std::vector<unsigned short>	mwpctac_list;	///< TAC time from the MWPC
	std::vector<long>			mwpctd_list;	///< list of ELUM time differences for ELUMFinder
	std::vector<char>			mwpcaxis_list;	///< list of axis IDs for the MWPC
	std::vector<char>			mwpcid_list;	///< list of TAC IDs for the MWPC

	// ELUM variables
	std::vector<float>	een_list;	///< list of ELUM energies for ELUMFinder
	std::vector<long>	etd_list;	///< list of ELUM time differences for ELUMFinder
	std::vector<char>	esec_list;	///< list of ELUM sectors for ELUMFinder

	// ZeroDegree variables
	std::vector<float>	zen_list;	///< list of ZeroDegree energies for ELUMFinder
	std::vector<long>	ztd_list;	///< list of ZeroDegree time differences for ELUMFinder
	std::vector<char>	zid_list;	///< list of ZeroDegree IDs/layers for ELUMFinder

	// ScintArray variables
	std::vector<float>	saen_list;	///< list of ScintArray energies for GammaFinder
	std::vector<long>	satd_list;	///< list of ScintArray time differences for GammaFinder
	std::vector<char>	said_list;	///< list of ScintArray detectors ids for GammaFinder

	// Counters
	unsigned int		hit_ctr;		///< Counts the number of hits that make up an event within a given file
	unsigned int		array_ctr;		///< Counts the number of real events (must have n and p-side signals) on the array within a given file
	unsigned int		arrayp_ctr;		///< Counts the number of real p-side events on the array within a given file
	unsigned int		recoil_ctr;		///< Counts the number of recoil events in the recoil detector within a given file
	unsigned int		mwpc_ctr;		///< Counts the number of MWPC events within a given file
	unsigned int		elum_ctr;		///< Counts the number of ELUM events within a given file
	unsigned int		zd_ctr;			///< Counts the number of zero-degree detector events within a given file
	unsigned int		gamma_ctr;		///< Counts the number of Gamma-Ray events within a given file
	unsigned long		n_asic_data;	///< Counter for the number of asic data packets in a file
	unsigned long		n_caen_data;	///< Counter for number of caen data packets in a file
	unsigned long		n_info_data; 	///< Counter for number of info data packets in a file
	unsigned long long	n_entries; 		///< Number of entries in the time-sorted data input tree
	unsigned long		n_caen_pulser;	///< Number of caen pulser hits in the time-sorted data input tree
	unsigned long		n_ebis;			///< Number of ebis pulses in the time-sorted data input tree
	unsigned long		n_t1; 			///< Number of T1 pulses in the time-sorted data input tree
	unsigned long		n_sc; 			///< Number of SuperCycle pulses in the time-sorted data input tree
	unsigned long		n_laser; 		///< Number of laser status pulses in the time-sorted data input tree
	std::vector<unsigned long>	n_fpga_pulser;	///< Number of fpga pulses in the time-sorted data input tree (indexed by module in the array)
	std::vector<unsigned long>	n_asic_pause;	///< Number of asic pause signals in the time-sorted data input tree (indexed by module in the array)
	std::vector<unsigned long>	n_asic_resume;	///< Number of asic resume signals in the time-sorted data input tree (indexed by module in the array)
	std::vector<unsigned long>	n_asic_pulser;	///< Number of asic pulses in the time-sorted data input tree (indexed by module in the array)

	// Array Histograms
	std::vector<std::vector<TH2F*>> pn_11;		///< Vector of vector of 2D histograms holding events with 1p and 1n hit
	std::vector<std::vector<TH2F*>> pn_12;		///< Vector of vector of 2D histograms holding events with 1p and 2n hits
	std::vector<std::vector<TH2F*>> pn_21;		///< Vector of vector of 2D histograms holding events with 2p and 1n hits
	std::vector<std::vector<TH2F*>> pn_22;		///< Vector of vector of 2D histograms holding events with 2p and 2n hits
	std::vector<std::vector<TH2F*>> pn_ab;		///< Vector of vector of 2D histograms with addback on p and n side
	std::vector<std::vector<TH2F*>> pn_nab;		///< Vector of vector of 2D histograms with p singles and n addback
	std::vector<std::vector<TH2F*>> pn_pab;		///< Vector of vector of 2D histograms with p addback and n singles
	std::vector<std::vector<TH2F*>> pn_max;		///< Vector of vector of 2D histograms with p and n-side max energy
	std::vector<std::vector<TH1F*>> pn_td;		///< Vector of vector of 1D histograms with p vs n side time difference
	std::vector<std::vector<TH1F*>> pp_td;		///< Vector of vector of 1D histograms with p-side time differences
	std::vector<std::vector<TH1F*>> nn_td;		///< Vector of vector of 1D histograms with n-side time differences
    std::vector<std::vector<TH2F*>> pn_td_Ep;	///< Vector of vector of 2D histograms pn-time difference vs p-side energy
    std::vector<std::vector<TH2F*>> pn_td_En;	///< Vector of vector of 2D histograms pn-time difference vs n-side energy
	std::vector<std::vector<TH2F*>> pn_mult;	///< Vector of vector of 2D histograms p-side vs n-side multiplicity
	
	std::vector<std::vector<TH1F*>> pn_td_prompt; ///< Vector of vector of 1D histograms with p vs n side time difference (prompt coincidence imposed)
	std::vector<std::vector<TH1F*>> pp_td_prompt; ///< Vector of vector of 1D histograms with p-side time differences (prompt coincidence imposed)
	std::vector<std::vector<TH1F*>> nn_td_prompt; ///< Vector of vector of 1D histograms with n-side time differences (prompt coincidence imposed)
	
	// Timing histograms
	TH1F *tdiff;					///< Histogram containing the time difference between each real (not infodata) signal in the file
	TH1F *tdiff_clean;				///< Histogram containing the time difference between the real signals *above threshold* (mythres)
	TH1F *caen_period;				///< Histogram of the CAEN pulser period
	TH1F *ebis_period;				///< Histogram containg the period of ebis pulses
	TH1F *t1_period;				///< Histogram containg the period of T1 pulses
	TH1F *sc_period;				///< Histogram containg the period of SuperCycle pulses
	TH1F *laser_period;				///< Histogram containg the period of Laser status signals
	TH1F *supercycle;				///< Histogram of T1 - SuperCycle time to get the super cycle structure
	std::vector<TH1F*> fpga_td; 	///<
	
	std::vector<TH1F*> asic_td; 				///< Histogram containing the time difference between ASIC signals for a given module of the array
	std::vector<TProfile*> fpga_pulser_loss;	///< TProfile counting the difference between the number of FPGA pulses and CAEN pulses as a function of FPGA time for a given module of the array
	std::vector<TH1F*> fpga_period; 			///<  Histogram containing the FPGA period as a function of FPGA time for a given module of the array
	std::vector<TProfile*> fpga_sync;			///< TProfile containing the time difference between FPGA pulses as a function of FPGA time for a given module of the array
	std::vector<TProfile*> asic_pulser_loss;	///< TProfile counting the difference between the number of ASIC pulses and CAEN pulses as a function of ASIC time for a given module of the array
	std::vector<TH1F*> asic_period;				///<  Histogram containing the ASIC period as a function of ASIC time for a given module of the array
	std::vector<TProfile*> asic_sync;			///< TProfile containing the time difference between ASIC pulses as a function of ASIC time for a given module of the array

	// Recoil histograms
	std::vector<TH2F*> recoil_EdE;				///< Histogram for the recoil E-dE that are real (calibrated)
	std::vector<TH2F*> recoil_dEsum;			///< Histogram for the recoil E+dE vs E (calibrated)
	std::vector<TH2F*> recoil_EdE_raw;			///< Histogram for the recoil E-dE that are real (raw)
	std::vector<TH1F*> recoil_E_singles;		///< Histogram containing the single E signals
	std::vector<TH1F*> recoil_dE_singles;		///<  Histogram containing the single dE signals
	std::vector<TH1F*> recoil_E_dE_tdiff;		///<  Histogram calculating the time difference between E and dE signals
	std::vector<TH2F*> recoil_tdiff;			///< Histogram for the recoil-recoil time differences as a function of layer number

	// MWPC histograms
	std::vector<std::vector<TH1F*>> mwpc_tac_axis; ///< The TAC singles spectra in the MWPC
	std::vector<TH1F*> mwpc_hit_axis; ///< The TAC difference spectra in the MWPC
	TH2F *mwpc_pos; ///< The TAC differences for multiplicity-2 events

	// ELUM histograms
	TH1F *elum_E; ///< The elum spectrum histogram
	TH2F *elum_E_vs_sec; ///< The elum spectrum histogram

	// ZeroDegree histograms
	TH2F *zd_EdE; ///< The zero-degree detector histogram

	// GammaRay histograms
	TH1F *gamma_E;			///< Sum gamma-ray energy histogram
	TH2F *gamma_E_vs_det;	///< Gamma-ray energy verus detector ID
	TH2F *gamma_gamma_E;	///< Gamma-gamma matrix, no prompt time condition
	TH1F *gamma_gamma_td;	///< Gamma-gamma time difference

};

#endif

