SCHEMA-ALLERGOEYE 001-000-000

USEINHERITANCE

ODML 2.5


ENUM DBEYE_ENUM
BEGIN
	"left"
	"right"
	"both sides"
END

ENUM DB_ALLERGEN
BEGIN
	"mite farinae"
	"mite pteronyssinus"
	"birch"
	"alder"
	"hazel"
	"ash tree"
	"grass"
	"mugwort"
	"ambrosia"
	"altenaria altenata"
	"penicillium notatum"
	"cat"
END

ENUM DB_DILUTION
BEGIN
	"start test"
	"control solution"
	"1-1000"
	"1-100"
	"1-10"
	"stock solution"
END

CLASS  DBCAP TABLE DBCAP
BEGIN
	AUTOID       oID       oID      SQL_BINARY    FREEINDEX
	ATOM_FIELD
		BEGIN
			oID              oID               SQL_BINARY	PRIMARYKEY COMMENT "Object ID"
			Allergen         Allergen          SQL_VARCHAR (DB_ALLERGEN)
			CAP              CAP               SQL_INTEGER
		END
	TRIGGERS BEGIN
	END
END

CLASS  DBDate TABLE DBDate
BEGIN
	AUTOID       oID       oID      SQL_BINARY    FREEINDEX
	ATOM_FIELD
		BEGIN
			oID              oID               SQL_BINARY	PRIMARYKEY COMMENT "Object ID"
			Allergen         Allergen          SQL_VARCHAR (DB_ALLERGEN)
			Date             Date              SQL_DATE
			Path             Path              SQL_NVARCHAR
		END

	COMPLEX_FIELD POINTER
		BEGIN
			Tests            DBTest            ARRAYOFOBJECTS CASCADEDELETE
			BRIDGE_TABLE _PBRcd1_DBDate_Tests_DBTest
			BEGIN
				HOLDER	HOLDERID	oID
				HANDLER	HANDLERID	oID
			END
		END
	TRIGGERS BEGIN
		LOADTRIGGER BEGIN
		".Allergen = instance WString(.Allergen);
"
		END
	END
END

CLASS  DBPatient TABLE DBPatient COMMENT "Patient record"
BEGIN
	AUTOID       oID       oID      SQL_BINARY    FREEINDEX
	ATOM_FIELD
		BEGIN
			oID              oID               SQL_BINARY	PRIMARYKEY COMMENT "Object ID"
			DateOfBirth      DateOfBirth       SQL_DATE
			FirstName        FirstName         SQL_NVARCHAR
			LastName         LastName          SQL_NVARCHAR
			Path             Path              SQL_NVARCHAR
			PatientIDStr     PatientIDStr      SQL_VARCHAR
		END

	COMPLEX_FIELD
		BEGIN
			CAP              DBCAP             ARRAYOFOBJECTS CASCADEREAD CASCADEDELETE
			BRIDGE_TABLE _PBR75d_DBPatient_CAP_DBCAP
			BEGIN
				HOLDER	HOLDERID	oID
				HANDLER	HANDLERID	oID
			END
		END

	COMPLEX_FIELD POINTER
		BEGIN
			Date             DBDate            ARRAYOFOBJECTS CASCADEDELETE
			BRIDGE_TABLE _PBR001_DBPatient_Date_DBDate
			BEGIN
				HOLDER	HOLDERID	oID
				HANDLER	HANDLERID	oID
			END
		END

	COMPLEX_FIELD POINTER
		BEGIN
			Reports          DBReport          ARRAYOFOBJECTS CASCADEDELETE
			BRIDGE_TABLE _PBR44c_tient_Reports_DBReport
			BEGIN
				HOLDER	HOLDERID	oID
				HANDLER	HANDLERID	oID
			END
		END

	COMPLEX_FIELD POINTER
		BEGIN
			Therapy          DBTherapy         ARRAYOFOBJECTS CASCADEDELETE
			BRIDGE_TABLE _PBRc55_ient_Therapy_DBTherapy
			BEGIN
				HOLDER	HOLDERID	oID
				HANDLER	HANDLERID	oID
			END
		END
	TRIGGERS BEGIN
		LOADTRIGGER BEGIN
		" "
		END
		DELETETRIGGER BEGIN
		" "
		END
	END
END

CLASS  DBPicture TABLE DBPicture
BEGIN
	AUTOID       oID       oID      SQL_BINARY    FREEINDEX
	ATOM_FIELD
		BEGIN
			oID              oID               SQL_BINARY	PRIMARYKEY COMMENT "Object ID"
			AreaL            AreaL             SQL_FLOAT NULL
			AreaR            AreaR             SQL_FLOAT NULL
			Contur           Contur            SQL_BIT
			Date             Date              SQL_DATE
			DistrL           DistrL            SQL_LONGVARBINARY NULL
			DistrR           DistrR            SQL_LONGVARBINARY NULL
			DistrTotal       DistrTotal        SQL_LONGVARBINARY NULL
			Names            Names             SQL_BIT
			Path             Path              SQL_NVARCHAR
			Result           Result            SQL_BIT
			ScoreL           ScoreL            SQL_FLOAT NULL
			ScoreR           ScoreR            SQL_FLOAT NULL
			Side             Side              SQL_VARCHAR (DBEYE_ENUM)
			Time             Time              SQL_TIME
		END
	TRIGGERS BEGIN
		SYSSAVETRIGGER BEGIN
		" if (.AreaL == EMPTY) .AreaL = -1e37;  if (.AreaR == EMPTY) .AreaR = -1e37;  if (.ScoreL == EMPTY) .ScoreL = -1e37;  if (.ScoreR == EMPTY) .ScoreR = -1e37; "
		END
		LOADTRIGGER BEGIN
		".Side =  instance WString(.Side);
"
		END
	END
END

CLASS  DBReport TABLE DBReport
BEGIN
	AUTOID       oID       oID      SQL_BINARY    FREEINDEX
	ATOM_FIELD
		BEGIN
			oID              oID               SQL_BINARY	PRIMARYKEY COMMENT "Object ID"
			Path             Path              SQL_NVARCHAR
		END
	TRIGGERS BEGIN
	END
END

CLASS  DBTest TABLE DBTest
BEGIN
	AUTOID       oID       oID      SQL_BINARY    FREEINDEX
	ATOM_FIELD
		BEGIN
			oID              oID               SQL_BINARY	PRIMARYKEY COMMENT "Object ID"
			AEScoreL         AEScoreL          SQL_FLOAT NULL
			AEScoreR         AEScoreR          SQL_FLOAT NULL
			Allergen         Allergen          SQL_VARCHAR (DB_ALLERGEN)
			Dil              Dil               SQL_VARCHAR (DB_DILUTION)
			DistrL           DistrL            SQL_LONGVARBINARY NULL
			DistrR           DistrR            SQL_LONGVARBINARY NULL
			DistrTotal       DistrTotal        SQL_LONGVARBINARY NULL
			Incubation       Incubation        SQL_NVARCHAR NULL
			OpScore          OpScore           SQL_FLOAT NULL
			OpScoreV         OpScoreV          SQL_VARBINARY NULL
			Path             Path              SQL_NVARCHAR
			Side             Side              SQL_VARCHAR (DBEYE_ENUM)
		END

	COMPLEX_FIELD POINTER
		BEGIN
			Pictures         DBPicture         ARRAYOFOBJECTS CASCADEDELETE
			BRIDGE_TABLE _PBRd0a_est_Pictures_DBPicture
			BEGIN
				HOLDER	HOLDERID	oID
				HANDLER	HANDLERID	oID
			END
		END
	TRIGGERS BEGIN
		SYSSAVETRIGGER BEGIN
		" if (.AEScoreL == EMPTY) .AEScoreL = -1e37;  if (.AEScoreR == EMPTY) .AEScoreR = -1e37;  if (.OpScore == EMPTY) .OpScore = -1e37; "
		END
		SAVETRIGGER BEGIN
		" "
		END
		LOADTRIGGER BEGIN
		".Dil = instance WString(.Dil);
.Side =  instance WString(.Side);
.Allergen = instance WString(.Allergen);
"
		END
		DELETETRIGGER BEGIN
		" "
		END
	END
END

CLASS  DBTherapy TABLE DBTherapy
BEGIN
	AUTOID       oID       oID      SQL_BINARY    FREEINDEX
	ATOM_FIELD
		BEGIN
			oID              oID               SQL_BINARY	PRIMARYKEY COMMENT "Object ID"
			Allergen         Allergen          SQL_VARCHAR (DB_ALLERGEN)
			Medication       Medication        SQL_NVARCHAR NULL
			StartDate        StartDate         SQL_DATE
		END
	TRIGGERS BEGIN
		LOADTRIGGER BEGIN
		".Allergen = instance WString(.Allergen);"
		END
	END
END

