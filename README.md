## BES Spectra 


### Obtaining this software
On an rcas node : 
```bash
git clone https://github.com/jdbrice/BESSpectra.git
cd BESSpectra
starver SL14i
cons
```

The code should now be built. Before running configure the event and track level cuts by editing cuts.xml (see example below)

### Creating QA plots only
```bash
root4star -b -q -l 'scripts/baseQA.C( "path/to/MuDst/list/file.lis", "output_suffix.root", max_num_events, max_num_files )'
```

### Creating picoDsts from MuDsts only
```bash
root4star -b -q -l 'scripts/dataPicos.C( "path/to/MuDst/list/file.lis", "output_suffix.root", max_num_events, max_num_files )'
```

### Creating QA and PicoDsts
```bash
root4star -b -q -l 'scripts/make.C( "path/to/MuDst/list/file.lis", "output_suffix.root", max_num_events, max_num_files )'
```

### Submitting jobs
Each of the above cases has a file in 'submit/' with the same name for submitting the corresponding job to the cluster. Just set the output paths.
For example, to submit a job making QA plots to the cluster :
```bash
star-submit submit/baseQA.xml
```


### Example cuts.xml file

```xml
<?xml version="1.0" encoding="UTF-8"?>

<StRcpSkimmerCuts>

	<!-- inclusive bounds -->

	<!-- Debadeepti's Analysis Event Cuts -->
	<Event>
		<triggers>440015</triggers>
		<zVertex min="-30" max="30" />
		<rVertex min="0" max="1.0" />
		<rVertexOffset x="0.0" y="0.89" /> <!-- These are **added** to the vertex before applying cut -->
		<nTofMatch min="0" max="30000" />
	</Event>


	<!-- Debadeepti's track cuts -->
	<Track> 
		<nHitsFit min="25" max="1000"/> 					<!-- nominally min=16 -->
		<nHitsDedx min="15" max="1000"/> 					<!-- nominally min=11 -->
		<nHitsRatio min="0.52" max="10.0"/> 				<!-- nominally min=0.52 -->


		<pt min="0.1" max="10"/>
		<ptRatio min="-100.1" max="102.42857"/>  						<!-- nominally : min="0.7" max="1.42857" -->
		<dca min="0" max="3.0"/> 										<!-- nominally max=1.0 -->
		<flag min="0" max="10000"/> 									<!-- nominally : min=0, max=700  -->
		<!--<rapidity min="-0.1" max="0.1" mass="0.13957018"/> --> 		<!-- nominally min="-0.25" max="0.25" mass="GeV/c" -->
		<pseudorapidity min="-1.0" max="1.0" /> 						<!-- nominally none, rapidity cut only"-->

	</Track>

	<!-- for setting up bounds on good / bad run plots -->
	<RunRange min="15000000" max="15072000"/>


	<!-- For QA purposes -->
	<!--  From http://www.star.bnl.gov/protected/lfspectra/jdb/run14/AuAu15/RefMultCorr/BadRunList.txt -->
	<!--  Day 52 and before should be completely excluded -->
	<BadRuns>
		15053027, 15053028, 15053029, 15053034, 15053035, 15053052, 15053054, 15053055, 15054053, 15054054, 15055018, 15055137, 15056117, 15057055, 15057059, 15058006, 15058011, 15058021, 15059057, 15059058, 15061001, 15061009, 15062006, 15062069, 15065012, 15065014, 15066070, 15068013, 15068014, 15068016, 15068018, 15069036, 15070008, 15070009, 15070010
	</BadRuns>

</StRcpSkimmerCuts>
```