VERSION NONAME 001-000-000

ODML 2.5

SUBPROJECT ALLERGOEYE 001-000-000
{
	DBPatient,
	DBTest,
	DBPicture,
	DBDate,
	DBReport,
	DBEYE_ENUM,
	DBTherapy,
	DB_DILUTION,
	DB_ALLERGEN,
	DBCAP
}

USEUUID_OBJECT_ID

ENUMERATED DBEYE_ENUM
  {
     "left",
     "right",
     "both sides" }

ENUMERATED DB_DILUTION
  {
     "start test",
     "control solution",
     "1-1000",
     "1-100",
     "1-10",
     "stock solution" }

ENUMERATED DB_ALLERGEN
  {
     "mite farinae",
     "mite pteronyssinus",
     "birch",
     "alder",
     "hazel",
     "ash tree",
     "grass",
     "mugwort",
     "ambrosia",
     "altenaria altenata",
     "penicillium notatum",
     "cat" }

CLASS DBPatient	COMMENT: "Patient record"
	ATTRIBUTE oID:	autoid unique BINARY(16)	COMMENT:"Object ID"
	ATTRIBUTE FirstName:	NVARCHAR(80)
	ATTRIBUTE LastName:	indexed NVARCHAR(80)
	ATTRIBUTE DateOfBirth:	DATE
	ATTRIBUTE PatientIDStr:	unique indexed VARCHAR(16)
	ATTRIBUTE Date:	pointer set-of cascade_del DBDate
	ATTRIBUTE Reports:	pointer set-of cascade_del DBReport
	ATTRIBUTE Path:	unique indexed NVARCHAR(255)
	ATTRIBUTE Therapy:	pointer set-of cascade_del DBTherapy
	ATTRIBUTE CAP:	set-of cascade_del cascade_read DBCAP
	LOAD_TRIGGER <| |>
	DELETE_TRIGGER <| |>

CLASS DBTest
	ATTRIBUTE oID:	autoid unique BINARY(16)	COMMENT:"Object ID"
	ATTRIBUTE Allergen:	DB_ALLERGEN
	ATTRIBUTE Pictures:	pointer set-of cascade_del DBPicture
	ATTRIBUTE Dil:	DB_DILUTION
	ATTRIBUTE Side:	DBEYE_ENUM
	ATTRIBUTE Incubation:	nullable NVARCHAR(10)
	ATTRIBUTE Path:	NVARCHAR(255)
	ATTRIBUTE OpScore:	nullable FLOAT  default : "-1e37"
	ATTRIBUTE OpScoreV:	nullable VARBINARY(254)
	ATTRIBUTE AEScoreL:	nullable FLOAT  default : "-1e37"
	ATTRIBUTE AEScoreR:	nullable FLOAT  default : "-1e37"
	ATTRIBUTE DistrL:	nullable LONGVARBINARY
	ATTRIBUTE DistrR:	nullable LONGVARBINARY
	ATTRIBUTE DistrTotal:	nullable LONGVARBINARY
	LOAD_TRIGGER <|.Dil = instance WString(.Dil);
.Side =  instance WString(.Side);
.Allergen = instance WString(.Allergen);
|>
	DELETE_TRIGGER <| |>
	SAVE_TRIGGER <| |>

CLASS DBPicture
	ATTRIBUTE oID:	autoid unique BINARY(16)	COMMENT:"Object ID"
	ATTRIBUTE Date:	DATE
	ATTRIBUTE Time:	TIME
	ATTRIBUTE Contur:	BOOLEAN
	ATTRIBUTE Names:	BOOLEAN
	ATTRIBUTE Result:	BOOLEAN
	ATTRIBUTE Path:	NVARCHAR(255)
	ATTRIBUTE Side:	DBEYE_ENUM
	ATTRIBUTE ScoreL:	nullable FLOAT  default : "-1e37"
	ATTRIBUTE ScoreR:	nullable FLOAT  default : "-1e37"
	ATTRIBUTE AreaL:	nullable FLOAT  default : "-1e37"
	ATTRIBUTE AreaR:	nullable FLOAT  default : "-1e37"
	ATTRIBUTE DistrL:	nullable LONGVARBINARY
	ATTRIBUTE DistrR:	nullable LONGVARBINARY
	ATTRIBUTE DistrTotal:	nullable LONGVARBINARY
	LOAD_TRIGGER <|.Side =  instance WString(.Side);
|>

CLASS DBDate
	ATTRIBUTE oID:	autoid unique BINARY(16)	COMMENT:"Object ID"
	ATTRIBUTE Date:	DATE
	ATTRIBUTE Tests:	pointer set-of cascade_del DBTest
	ATTRIBUTE Path:	NVARCHAR(255)
	ATTRIBUTE Allergen:	DB_ALLERGEN
	LOAD_TRIGGER <|.Allergen = instance WString(.Allergen);
|>

CLASS DBReport
	ATTRIBUTE oID:	autoid unique BINARY(16)	COMMENT:"Object ID"
	ATTRIBUTE Path:	NVARCHAR(255)

CLASS DBTherapy
	ATTRIBUTE oID:	autoid unique BINARY(16)	COMMENT:"Object ID"
	ATTRIBUTE StartDate:	DATE
	ATTRIBUTE Allergen:	DB_ALLERGEN
	ATTRIBUTE Medication:	nullable NVARCHAR(80)
	LOAD_TRIGGER <|.Allergen = instance WString(.Allergen);|>

CLASS DBCAP
	ATTRIBUTE oID:	autoid unique BINARY(16)	COMMENT:"Object ID"
	ATTRIBUTE Allergen:	DB_ALLERGEN
	ATTRIBUTE CAP:	INTEGER

