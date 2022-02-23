/*********************************************************
** White light interferometry functions					**
*********************************************************/
/*!
	\fn short  __stdcall MCHR_GetSAWLINumberOfLayer(MCHR_ID SensorID, long *piNbLayers)
	\if FR
		\brief Fonction permettant de recuperer le nombre de couches mesurable en mode ILB (SAWLI).
		\warning Seulement pour les coffrets ILB (SAWLI).
	\else
		\brief Function getting number of layers measurable in SAWLI mode.
		\warning Only for SAWLI Controllers.
	\endif
	\details
		\if FR
			\param [in] SensorID : ID du coffret.
			\param [out] piNbLayers : Nombre de couche mesurable.
			\return short : un code d'erreur MCHR_ERROR.
		\else
			\param [in] SensorID : ID of the device.
			\param [out] piNbLayers : Number of SAWLI measurable layers.
			\return short : A \link MCHR_ERROR MCHR_ERROR \endlink code.
		\endif
		
*/
DLL_CHR_API short  __stdcall MCHR_GetSAWLINumberOfLayer(MCHR_ID SensorID, long *piNbLayers);

/*!
	\fn short  __stdcall MCHR_SetSAWLINumberOfLayer(MCHR_ID SensorID, long iNbLayers)
	\if FR
		\brief Fonction permettant de paramétrer le nombre de couches mesurable en mode ILB (SAWLI).
		\warning Seulement pour les coffrets ILB (SAWLI).
	\else
		\brief Function setting number of layers measurable in SAWLI mode.
		\warning Only for SAWLI Controllers.
	\endif
	\details
		\if FR
			\param [in] SensorID : ID du coffret.
			\param [in] iNbLayers : Nombre de couche mesurable.
			\return short : un code d'erreur MCHR_ERROR.
		\else
			\param [in] SensorID : ID of the device.
			\param [in] iNbLayers : Number of SAWLI measurable layers.
			\return short : A \link MCHR_ERROR MCHR_ERROR \endlink code.
		\endif
*/
DLL_CHR_API short  __stdcall MCHR_SetSAWLINumberOfLayer(MCHR_ID SensorID, long iNbLayers);

/*!
	\fn short  __stdcall MCHR_GetSAWLISmoothing(MCHR_ID SensorID, long *piSmoothing)
	\if FR
		\brief Fonction permettant de recuperer le lissage du signal en mode ILB (SAWLI).
		\warning Seulement pour les coffrets ILB (SAWLI).
	\else
		\brief Function getting smoothing value in SAWLI mode.
		\warning Only for SAWLI Controllers.
	\endif
	\details
		\if FR
			\param [in] SensorID : ID du coffret.
			\param [out] piSmoothing : Largeur du lissage.
			\return short : un code d'erreur MCHR_ERROR.
		\else
			\param [in] SensorID : ID of the device.
			\param [out] piSmoothing : Smoothing width.
			\return short : A \link MCHR_ERROR MCHR_ERROR \endlink code.
		\endif
*/
DLL_CHR_API short  __stdcall MCHR_GetSAWLISmoothing(MCHR_ID SensorID, long *piSmoothing);

/*!
	\fn short  __stdcall MCHR_SetSAWLISmoothing(MCHR_ID SensorID, long iSmoothing)
	\if FR
		\brief Fonction permettant de parametrer le lissage du signal en mode ILB (SAWLI).
		\warning Seulement pour les coffrets ILB (SAWLI).
	\else
		\brief Function setting smoothing value in SAWLI mode.
		\warning Only for SAWLI Controllers.
	\endif
	\details
		\if FR
			\param [in] SensorID : ID du coffret.
			\param [in] iSmoothing : Largeur du lissage.
			\return short : un code d'erreur MCHR_ERROR.
		\else
			\param [in] SensorID : ID of the device.
			\param [in] iSmoothing : Smoothing width.
			\return short : A \link MCHR_ERROR MCHR_ERROR \endlink code.
		\endif
*/
DLL_CHR_API short  __stdcall MCHR_SetSAWLISmoothing(MCHR_ID SensorID, long iSmoothing);

//These 4 functions helps you define the measurement range
// en micron

/*! 
	\fn short  __stdcall MCHR_GetSAWLIMinThickness(MCHR_ID SensorID, double *piThickness)
	\if FR
		\brief Get the minimal value (threshold)  of  thickness to detect
		\warning Seulement pour les coffrets ILB (SAWLI).
	\else
		\brief Get the minimal value (threshold)  of  thickness to detect
		\warning Only for SAWLI Controllers.
	\endif
	\details
		\if FR
			\param [in] SensorID: Sensor Identifier  
			\param [in] piThickness: pointer to the Min thickness threshold.
		\else
			\param [in] SensorID: Sensor Identifier  
			\param [in] piThickness: pointer to the Min thickness threshold.
		\endif
		\return short : A \link MCHR_ERROR MCHR_ERROR \endlink code.
*/
DLL_CHR_API short  __stdcall MCHR_GetSAWLIMinThickness(MCHR_ID SensorID,double *piThickness);

/*! 
	\fn short  __stdcall MCHR_SetSAWLIMinThickness(MCHR_ID SensorID, double iThickness)
	\if FR
		\brief Set the minimal value (threshold)  of  thickness to detect
		\warning Seulement pour les coffrets ILB (SAWLI).
	\else
		\brief Set the minimal value (threshold)  of  thickness to detect
		\warning Only for SAWLI Controllers.
	\endif
	\details
		\if FR
			\param [in] SensorID: Sensor Identifier  
			\param [in] iThickness: Min thickness threshold, in µm.
		\else
			\param [in] SensorID: Sensor Identifier  
			\param [in] iThickness: Min thickness threshold, in µm.
		\endif
		\return short : A \link MCHR_ERROR MCHR_ERROR \endlink code.
*/
DLL_CHR_API short  __stdcall MCHR_SetSAWLIMinThickness(MCHR_ID SensorID, double iThickness);

/*! 
	\fn short  __stdcall MCHR_GetSAWLIMaxThickness(MCHR_ID SensorID, double *piThickness)
	\if FR
		\brief Get the maximal value (threshold)  of  thickness to detect
		\warning Seulement pour les coffrets ILB (SAWLI).
	\else
		\brief Get the maximal value (threshold)  of  thickness to detect
		\warning Only for SAWLI Controllers.
	\endif
	\details
		\if FR
			\param [in] SensorID: Sensor Identifier  
			\param [out] piThickness: pointer to the Max thickness threshold.
		\else
			\param [in] SensorID: Sensor Identifier  
			\param [out] piThickness: pointer to the Max thickness threshold.
		\endif
		\return short : A \link MCHR_ERROR MCHR_ERROR \endlink code.
*/
DLL_CHR_API short  __stdcall MCHR_GetSAWLIMaxThickness(MCHR_ID SensorID, double *piThickness);

/*! 
	\fn short  __stdcall MCHR_SetSAWLIMaxThickness(MCHR_ID SensorID, double iThickness)
	\if FR
		\brief Set the maximal value (threshold)  of  thickness to detect
		\warning Seulement pour les coffrets ILB (SAWLI).
	\else
		\brief Set the maximal value (threshold)  of  thickness to detect
		\warning Only for SAWLI Controllers.
	\endif
	\details
		\if FR
			\param [in] SensorID: Sensor Identifier  
			\param [in] iThickness: Max thickness threshold, in µm.
		\else
			\param [in] SensorID: Sensor Identifier  
			\param [in] iThickness: Max thickness threshold, in µm.
		\endif
		\return short : A \link MCHR_ERROR MCHR_ERROR \endlink code.

*/
DLL_CHR_API short  __stdcall MCHR_SetSAWLIMaxThickness(MCHR_ID SensorID, double iThickness);

//Indices de refraction en mode ILB mode
//Refractive index used in the SAWLI mode

/*! 
	\fn short  __stdcall MCHR_GetSAWLIRefractiveIndex(MCHR_ID SensorID,double *dRefractiveIndex);
	\if FR
		\brief Obtient l'indidice de refraction en mode interferometrique.
		\warning Seulement pour les coffrets ILB (SAWLI).
	\else
		\brief Get the refractive index in Interferometric mode.
		\warning Only for SAWLI Controllers.
	\endif
	\details
		\if FR
			\param [in] SensorID: Sensor Identifier  
			\param [out] dRefractiveIndex : Indice de refraction
		\else
			\param [in] SensorID: Sensor Identifier  
			\param [out] dRefractiveIndex : Index of refractive
		\endif
		\return short : A \link MCHR_ERROR MCHR_ERROR \endlink code.
*/
DLL_CHR_API short  __stdcall MCHR_GetSAWLIRefractiveIndex(MCHR_ID SensorID,double *dRefractiveIndex);

/*! 
	\fn short  __stdcall MCHR_SetSAWLIRefractiveIndex(MCHR_ID SensorID,double dRefractiveIndex);
	\if FR
		\brief Configure l'indidice de refraction en mode interferometrique.
		\warning Seulement pour les coffrets ILB (SAWLI).
	\else
		\brief Set the refractive index in Interferometric mode.
		\warning Only for SAWLI Controllers.
	\endif
	\details
		\if FR
			\param SensorID: Sensor Identifier  
			\param dRefractiveIndex : Indice de refraction
		\else
			\param SensorID: Sensor Identifier  
			\param dRefractiveIndex : Index of refractive
		\endif
		\return short : A \link MCHR_ERROR MCHR_ERROR \endlink code.
*/
DLL_CHR_API short  __stdcall MCHR_SetSAWLIRefractiveIndex(MCHR_ID SensorID,double dRefractiveIndex);

/*
 Fonction d'acquisition d'une sequence de points en mode interferometrique avec capteur MTS-DUO
 Acquisition function for the Interferometric mode 
 voir le .cpp correspondant pour plus d'info lors de la redaction de la doc
*/
/*!
	\fn short  __stdcall MCHR_GetInterferometricThicknessSAWLI ( MCHR_ID  SensorID, MCHR_tyAcqParam Parameters,	
													 PFLOAT *pArrayThickness1, PFLOAT *pArrayThickness2, PFLOAT *pArrayThickness3,
													 PFLOAT *pArrayQuality1,	PFLOAT *pArrayQuality2, PFLOAT *pArrayQuality3,
													 PFLOAT *pArrayIntensity,PFLOAT *pNbThickness,PFLOAT *pCounter,PFLOAT *pState); 
		\if FR
		\brief Acquisition function for the Specral Interferometric (SAWLI) Thickness mode.<br>
		\brief Note: in Distance mode set the number of layer to 1, and the refractive index to 1.0.<br>
        \brief The Distance Data may be comuted from the air gap thickness using the following raelation:<br>          
		\brief Distance = 160µm ?Thickness1.
		\warning Seulement pour les coffrets ILB (SAWLI).
	\else
		\brief Acquisition function for the Specral Interferometric (SAWLI) Thickness mode.<br>
		\brief Note: in Distance mode set the number of layer to 1, and the refractive index to 1.0.<br>
        \brief The Distance Data may be comuted from the air gap thickness using the following raelation:<br>          
		\brief Distance = 160µm ?Thickness1.
		\brief This function is destined to STIL-DUO in the interferometric mode
		\warning Only for SAWLI Controllers.
	\endif
	\details
		\if FR
			\param SensorID : Sensor Identifier  
			\param Parameters : data structure for setting the measurement parameters
			\param pArrayThickness1 : a pointer  to the Thickness1 data buffers.
			\param pArrayThickness2 : a pointer  to the Thickness2 data buffers.
			\param pArrayThickness3 : a pointer  to the Thickness3 data buffers
			\param pArrayQuality1 : a pointer  to the Quality1 data buffers
			\param pArrayQuality2 : a pointer  to the Quality2 data buffers
			\param pArrayQuality3 : a pointer  to the Quality3 data buffers
			\param pArrayIntensity : a pointer  to the Intensity data buffers
			\param pNbThickness : a pointer to the Number Of Thicknesses data Buffers
			\param pCounter : a pointer to the Counter data Buffers
			\param pState : a pointer to the State Data buffer
		\else
			\param SensorID : Sensor Identifier  
			\param Parameters : data structure for setting the measurement parameters
			\param pArrayThickness1 : a pointer  to the Thickness1 data buffers.
			\param pArrayThickness2 : a pointer  to the Thickness2 data buffers.
			\param pArrayThickness3 : a pointer  to the Thickness3 data buffers
			\param pArrayQuality1 : a pointer  to the Quality1 data buffers
			\param pArrayQuality2 : a pointer  to the Quality2 data buffers
			\param pArrayQuality3 : a pointer  to the Quality3 data buffers
			\param pArrayIntensity : a pointer  to the Intensity data buffers
			\param pNbThickness : a pointer to the Number Of Thicknesses data Buffers
			\param pCounter : a pointer to the Counter data Buffers
			\param pState : a pointer to the State Data buffer
		\endif
		\if FR 
			\par Capteurs Compatibles : STIL DUO en mode de mesure SAWLI interfermotric. 
		\else
			\par Compatibles Sensors : STIL DUO in the SAWLI interfermotric measuring modes.
		\endif
		\return short : A \link MCHR_ERROR MCHR_ERROR \endlink code.
*/
DLL_CHR_API short  __stdcall MCHR_GetInterferometricThicknessSAWLI  (MCHR_ID  SensorID, MCHR_tyAcqParam Parameters,	
													 PFLOAT *pArrayThickness1, PFLOAT *pArrayThickness2, PFLOAT *pArrayThickness3,
													 PFLOAT *pArrayQuality1,	PFLOAT *pArrayQuality2, PFLOAT *pArrayQuality3,
													 PFLOAT *pArrayIntensity,PFLOAT *pNbThickness,PFLOAT *pCounter,PFLOAT *pState);




