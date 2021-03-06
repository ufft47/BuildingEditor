#include "newbuildingwizard.h"
#include "ui_newbuildingwizard.h"
#include "generalwizardpage.h"
#include "specialwizardpage.h"
#include "omtwizardpage.h"

NewBuildingWizard::NewBuildingWizard(QWidget *parent) :
    QWizard(parent),
    ui(new Ui::NewBuildingWizard)
{
    ui->setupUi(this);

    setPage(GENERAL_PAGE, new GeneralWizardPage());
    setPage(SPECIAL_PAGE, new SpecialWizardPage());
    setPage(OMT_PAGE, new OMTWizardPage());

    setStartId(GENERAL_PAGE);
}

NewBuildingWizard::~NewBuildingWizard()
{
    delete ui;
}

bool NewBuildingWizard::IsExistingOMT()
{
    return field("ExistingOMT").toBool();
}

bool NewBuildingWizard::IsNewOMT()
{
    return field("NewOMT").toBool();
}

bool NewBuildingWizard::IsNewSpecial()
{
    return field("NewSpecial").toBool();
}

QVector<bool> NewBuildingWizard::GetLayout()
{
    if (field("NewSpecial").toBool())
    {
        return ((SpecialWizardPage*)page(SPECIAL_PAGE))->GetLayout();
    }
    QVector<bool> layout;
    layout.fill(false, 9 * 9 * 21);
    layout[0] = true;
    return layout;
}

OvermapSpecialData NewBuildingWizard::GetSpecialData()
{
    return ((SpecialWizardPage*)page(SPECIAL_PAGE))->GetData();
}

OMTData NewBuildingWizard::GetOMTData()
{
    return ((OMTWizardPage*)page(OMT_PAGE))->GetOMTData();
}

void NewBuildingWizard::Finalize()
{

}
